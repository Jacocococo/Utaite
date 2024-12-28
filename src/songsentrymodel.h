#ifndef SONGSENTRYMODEL_H
#define SONGSENTRYMODEL_H

#include <QAbstractListModel>

#include "songmodel.h"

class SongsEntryModel : public QAbstractListModel {
    Q_OBJECT
    Q_PROPERTY(QList<SongModel*> songs READ getSongs)

public:
    explicit SongsEntryModel(QList<SongModel*>& songs, QObject* parent = nullptr);

    enum {
        PlaylistIndexRole = Qt::UserRole + 1,
        FileRole,
        ImageRole,
        TitleRole,
        ArtistRole,
    };

    virtual QHash<int, QByteArray> roleNames() const override;

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    const QList<SongModel*> getSongs() const;

    Q_INVOKABLE void move(int oldIndex, int newIndex);
    Q_INVOKABLE void remove(int index);

public slots:
    void refresh();

signals:
    void songMoved(int oldIndex, int newIndex);
    void songRemoved(int index);

private:
    QList<SongModel*>& songs;
};

#endif // SONGSENTRYMODEL_H
