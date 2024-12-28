#ifndef SONGSINDEX_H
#define SONGSINDEX_H

#include <QObject>
#include <QDir>

#include "songmodel.h"

class SongsIndex : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QList<SongModel*> songs MEMBER songs)

public:
    SongsIndex(QObject* parent = nullptr);
    virtual ~SongsIndex();

    QList<SongModel*> songs;
    QFile mainPlaylistFile;

    void indexFiles(QDir& dir);
    void readPlaylistFile(QFile& file);
    void writePlaylistFile(QFile& file);

    QString writeImage(QByteArray& img);

    Q_INVOKABLE bool mainPlaylistFileExists();
    Q_INVOKABLE void indexFilesAndSaveMainPlaylist(QString musicPath);

signals:
    void updatedIndex();

private:
    QDir coverartCache;

    SongModel* readSongMetadata(int index, const char* fileName);
};

#endif // SONGSINDEX_H
