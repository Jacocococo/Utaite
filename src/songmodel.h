#ifndef SONGMODEL_H
#define SONGMODEL_H

#include <QObject>
#include <QUrl>

class SongModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(int playlistIndex MEMBER playlistIndex NOTIFY playlistIndexChanged FINAL)
    Q_PROPERTY(QUrl file MEMBER file NOTIFY fileChanged FINAL)
    Q_PROPERTY(QUrl image MEMBER image NOTIFY imageChanged FINAL)
    Q_PROPERTY(QString title MEMBER title NOTIFY titleChanged FINAL)
    Q_PROPERTY(QString artist MEMBER artist NOTIFY artistChanged FINAL)

public:
    explicit SongModel(
        int playlistIndex,
        QUrl file,
        QUrl image,
        QString title,
        QString artist,
        QObject* parent = nullptr
    );

    int playlistIndex;
    QUrl file;
    QUrl image;
    QString title;
    QString artist;

    QString imageToCachedUri();

signals:
    void playlistIndexChanged();
    void fileChanged();
    void imageChanged();
    void titleChanged();
    void artistChanged();
};

#endif // SONGMODEL_H
