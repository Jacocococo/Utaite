#include "songsindex.h"

#include <QImage>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QCryptographicHash>
#include <QMimeDatabase>
#include <QStandardPaths>

extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/dict.h>
}

SongsIndex::SongsIndex(QObject* parent)
    : QObject(parent)
    , mainPlaylistFile([]() {
        QDir appData(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
        if (!appData.exists())
            appData.mkpath(".");
        return QFile(appData.absoluteFilePath("main.xspf"));
    }())
    , coverartCache([]() {
        QDir coverartCache = QDir(QStandardPaths::writableLocation(QStandardPaths::CacheLocation));
        if (!coverartCache.exists("coverart"))
            coverartCache.mkpath("coverart");
        coverartCache.cd("coverart");
        return coverartCache;
    }())
{}

SongsIndex::~SongsIndex() {
    qDeleteAll(songs);
}

extern "C" {
void readMetadataDict(AVDictionary* metadata, QString& title, QString& artist) {
    AVDictionaryEntry* tag = NULL;

    while ((tag = av_dict_get(metadata, "", tag, AV_DICT_IGNORE_SUFFIX))) {
        if (qstricmp(tag->key, "title") == 0)
            title = tag->value;
        else if (qstricmp(tag->key, "artist") == 0)
            artist = tag->value;
    }
}

SongModel* SongsIndex::readSongMetadata(int index, const char* fileName) {
    AVFormatContext* context = NULL;

    avformat_open_input(&context, fileName, NULL, NULL);
    avformat_find_stream_info(context, NULL);

    QString title;
    QString artist;
    QByteArray image;

    readMetadataDict(context->metadata, title, artist);

    bool foundImage = false;
    for (unsigned int i = 0; i < context->nb_streams; i++) {
        const AVStream* stream = context->streams[i];
        if (!stream)
            continue;

        readMetadataDict(stream->metadata, title, artist);

        if (foundImage)
            continue;
        if (!(stream->disposition & AV_DISPOSITION_ATTACHED_PIC))
            continue;

        const AVPacket *attachedImage = &stream->attached_pic;

        if (!attachedImage || !attachedImage->data || attachedImage->size <= 0)
            continue;

        image = { (const char*) attachedImage->data, attachedImage->size };
        foundImage = true;
    }

    avformat_close_input(&context);

    return new SongModel(
        index,
        QUrl(fileName),
        QUrl::fromLocalFile(coverartCache.filePath(writeImage(image))),
        title,
        artist);
}
}

// TODO: Multi-threading
void SongsIndex::indexFiles(QDir& dir) {
    int i = 0;
    for (QString& file : dir.entryList(QDir::Files, QDir::Time | QDir::Reversed)) {
        SongModel* songModel = readSongMetadata(i, dir.absoluteFilePath(file).toUtf8().data());
        songs.append(songModel);
        i++;
    }

    emit updatedIndex();
}

void SongsIndex::readPlaylistFile(QFile& file) {
    if (!file.open(QIODevice::ReadOnly)) // TODO: Error handling
        return;

    QXmlStreamReader reader;
    reader.setDevice(&file);
    reader.readNextStartElement(); // playlist
    while (reader.readNextStartElement()) { // playlist elements
        if (reader.name() == "trackList") {
            int songNum = 0;
            while (reader.readNextStartElement() && reader.name() != "trackList") {
                QUrl location;
                QString title;
                QString artist;
                QUrl image;

                while (reader.readNextStartElement() && reader.name() != "track") { // track metadata
                    if (reader.name() == "location") {
                        location = QUrl(reader.readElementText());
                    } else if (reader.name() == "title") {
                        title = reader.readElementText();
                    } else if (reader.name() == "creator") {
                        artist = reader.readElementText();
                    } else if (reader.name() == "image") {
                        image = QUrl(reader.readElementText());
                    } else { // TODO: handle extension elements with xml body
                        reader.readElementText();
                    }
                }

                if (image.scheme() == "utaite-cache")
                    image = QUrl::fromLocalFile(coverartCache.filePath(image.fileName()));

                SongModel* songModel = new SongModel(songNum, location, image, title, artist);
                songs.append(songModel);

                songNum++;
            }
        }
    }

    emit updatedIndex();

    file.close();
}

void SongsIndex::writePlaylistFile(QFile& file) {
    if (!file.open(QIODevice::ReadWrite)) // TODO: Error handling
        return;

    QXmlStreamWriter writer(&file);
    writer.setAutoFormatting(true);
    writer.writeStartDocument();

    writer.writeStartElement("playlist");
    writer.writeAttribute("version", "1");
    writer.writeAttribute("xmlns", "http://xspf.org/ns/0/");
        writer.writeStartElement("trackList");
            for (SongModel* song : songs) {
                writer.writeStartElement("track");
                    writer.writeTextElement("location", song->file.toString());
                    writer.writeTextElement("title", song->title);
                    writer.writeTextElement("creator", song->artist);
                    writer.writeTextElement("image", song->imageToCachedUri());
                writer.writeEndElement();
            }
        writer.writeEndElement();
    writer.writeEndElement();

    writer.writeEndDocument();
    file.close();
}

QString SongsIndex::writeImage(QByteArray& data) {
    QCryptographicHash md5(QCryptographicHash::Md5);
    md5.addData(data);
    QString hash(md5.result().toHex());
    QString filetype = QMimeDatabase().mimeTypeForData(data).preferredSuffix();
    QString fileName = hash + "." + filetype;
    QImage img = QImage::fromData(data);
    if(!coverartCache.exists(fileName))
        img.save(coverartCache.absoluteFilePath(fileName));
    return fileName;
}

bool SongsIndex::mainPlaylistFileExists() {
    return mainPlaylistFile.exists();
}

void SongsIndex::indexFilesAndSaveMainPlaylist(QString musicPath) {
    QDir dir(musicPath);
    indexFiles(dir);
    writePlaylistFile(mainPlaylistFile);
}
