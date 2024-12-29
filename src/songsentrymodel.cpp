#include "songsentrymodel.h"

#include <QUrl>
#include <QList>

SongsEntryModel::SongsEntryModel(QList<SongModel*>& songs, QObject* parent)
    : QAbstractListModel(parent)
    , songs(songs)
{}

QHash<int, QByteArray> SongsEntryModel::roleNames() const {
    QHash<int, QByteArray> names;
    names[PlaylistIndexRole] = "playlistIndex";
    names[FileRole] = "file";
    names[ImageRole] = "image";
    names[TitleRole] = "title";
    names[ArtistRole] = "artist";
    return names;
}

int SongsEntryModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid())
        return 0;

    return songs.size();
}

QVariant SongsEntryModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid())
        return QVariant();

    SongModel* model = songs.at(index.row());
    switch(role) {
        case PlaylistIndexRole:
            return QVariant(model->playlistIndex);
        case FileRole:
            return QVariant(model->file);
        case ImageRole:
            return QVariant(model->image);
        case TitleRole:
            return QVariant(model->title);
        case ArtistRole:
            return QVariant(model->artist);
    }

    return QVariant();
}

const QList<SongModel*> SongsEntryModel::getSongs() const {
    return songs;
}

void SongsEntryModel::refresh() {
    beginResetModel();
    endResetModel();
}

void SongsEntryModel::move(int oldIndex, int newIndex) {
    int dest = newIndex > oldIndex ? newIndex + 1 : newIndex;
    beginMoveRows(QModelIndex(), oldIndex, oldIndex, QModelIndex(), dest);
    songs.swapItemsAt(oldIndex, newIndex);
    endMoveRows();
    emit songMoved(oldIndex, newIndex);
}

void SongsEntryModel::remove(int index) {
    beginRemoveRows(QModelIndex(), index, index);
    songs.removeAt(index);
    emit songRemoved(index);
    endRemoveRows();
}
