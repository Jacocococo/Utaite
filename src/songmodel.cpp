#include "songmodel.h"

SongModel::SongModel(
        int playlistIndex,
        QUrl file,
        QUrl image,
        QString title,
        QString artist,
        QObject* parent
    ) : QObject(parent)
    , playlistIndex(playlistIndex)
    , file(file)
    , image(image)
    , title(title)
    , artist(artist)
{}

QString SongModel::imageToCachedUri() {
    return "utaite-cache:" + image.fileName();
}
