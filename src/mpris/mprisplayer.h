#ifndef MPRISPLAYER_H
#define MPRISPLAYER_H

#include <QDBusAbstractAdaptor>
#include <QDBusObjectPath>

#include "../playlistmanager.h"

class MprisPlayer : public QDBusAbstractAdaptor
{
    Q_OBJECT

    Q_CLASSINFO("D-Bus Interface", "org.mpris.MediaPlayer2.Player")

    Q_PROPERTY(QString PlaybackStatus READ PlaybackStatus)
    Q_PROPERTY(QString LoopStatus READ LoopStatus WRITE setLoopStatus)
    Q_PROPERTY(qreal Rate READ Rate WRITE setRate)
    Q_PROPERTY(bool Shuffle READ Shuffle WRITE setShuffle)
    Q_PROPERTY(QVariantMap Metadata READ Metadata)
    Q_PROPERTY(qreal Volume READ Volume WRITE setVolume)
    Q_PROPERTY(qint64 Position READ Position)
    Q_PROPERTY(qreal MinimumRate READ MinimumRate)
    Q_PROPERTY(qreal MaximumRate READ MaximumRate)
    Q_PROPERTY(bool CanGoNext READ CanGoNext)
    Q_PROPERTY(bool CanGoPrevious READ CanGoPrevious)
    Q_PROPERTY(bool CanPlay READ CanPlay)
    Q_PROPERTY(bool CanPause READ CanPause)
    Q_PROPERTY(bool CanSeek READ CanSeek)
    Q_PROPERTY(bool CanControl READ CanControl CONSTANT)

public:
    MprisPlayer(PlaylistManager* playlist, QObject* parent = nullptr);

    QString PlaybackStatus() const;
    QString LoopStatus() const;
    void setLoopStatus(QString loopStatus);
    qreal Rate() const;
    void setRate(qreal rate);
    bool Shuffle() const;
    void setShuffle(bool shuffle);
    QVariantMap Metadata() const;
    qreal Volume() const;
    void setVolume(qreal volume);
    qint64 Position() const;
    qreal MinimumRate() const;
    qreal MaximumRate() const;
    bool CanGoNext() const;
    bool CanGoPrevious() const;
    bool CanPlay() const;
    bool CanPause() const;
    bool CanSeek() const;
    bool CanControl() const;

public slots:
    void Next();
    void Previous();
    void Pause();
    void PlayPause();
    void Stop();
    void Play();
    void Seek(qint64 position);
    void SetPosition(const QDBusObjectPath& trackID, qint64 position);
    void OpenUri(const QString& uri);

signals:
    void Seeked(qint64 position);

private:
    PlaylistManager* playlist;

    QString currentTrackID() const;

    void propertiesChanged(const QVariantMap& properties);
};

#endif // MPRISPLAYER_H
