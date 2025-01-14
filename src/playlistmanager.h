#ifndef PLAYLISTMANAGER_H
#define PLAYLISTMANAGER_H

#include <QMediaPlayer>
#include <QAudioOutput>

#include "songmodel.h"
#include "songsentrymodel.h"

class PlaylistManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(SongModel* currentSong READ currentSong NOTIFY songChanged)
    Q_PROPERTY(bool playing MEMBER playing NOTIFY playingChanged)
    Q_PROPERTY(qreal playbackPosition READ playbackPosition WRITE setPlaybackPosition NOTIFY playbackPositionChanged)
    Q_PROPERTY(QList<SongModel*> queue READ getQueue NOTIFY queueChanged)
    Q_PROPERTY(int queueCursor READ getQueueCursor NOTIFY queueCursorChanged)
    Q_PROPERTY(SongsEntryModel* queueModel READ getQueueModel NOTIFY queueChanged)
    Q_PROPERTY(bool canPlayNext READ canPlayNext NOTIFY queueCursorChanged)
    Q_PROPERTY(bool canPlayPrevious READ canPlayPrevious NOTIFY queueCursorChanged)
    Q_PROPERTY(bool shuffle READ isShuffling WRITE setShuffle NOTIFY shuffleChanged)

public:
    explicit PlaylistManager(QList<SongModel*>& songs, QObject* parent = nullptr);
    virtual ~PlaylistManager();

    Q_INVOKABLE void setSong(int index);

    Q_INVOKABLE void play();
    Q_INVOKABLE void pause();
    Q_INVOKABLE void togglePlay();
    Q_INVOKABLE void stop();

    Q_INVOKABLE void setQueueIndex(int index);

    bool canPlayNext() const;
    bool canPlayPrevious() const;

    Q_INVOKABLE void playNext();
    Q_INVOKABLE void playPrevious();

    Q_INVOKABLE void enqueue(int playlistIndex);
    Q_INVOKABLE void enqueueNext(int playlistIndex);

    Q_INVOKABLE void shuffleQueue();

    bool isPlaying() const;
    bool isSeekable() const;
    bool isShuffling() const;
    void setShuffle(bool shuffle);
    SongModel* currentSong() const;
    QList<SongModel*> getQueue() const;
    int getQueueCursor() const;
    SongsEntryModel* getQueueModel() const;

    qreal playbackPosition() const;
    void setPlaybackPosition(qreal pos);

    qint64 currentSongDuration() const;
    qint64 playbackPositionMs() const;
    void setPlaybackPositionMs(qint64 pos);

signals:
    void songChanged();
    void queueChanged();
    void queueCursorChanged();
    void playingChanged();
    void playbackPositionChanged();
    void shuffleChanged();

private:
    QList<SongModel*>& songs;
    QMediaPlayer* player;
    QAudioOutput* audioOutput;
    QList<SongModel*> queue;
    int queueCursor;
    SongsEntryModel* queueModel;
    bool playing = false;
    qreal position = 0;
    bool shuffle = true; // TODO: allow changing shuffle through a setting

private slots:
    void onMediaStatusChanged(QMediaPlayer::MediaStatus mediaStatus);
    void onPositionChanged(qint64 pos);
    void onSongChanged();
    void onSongMoved(int oldIndex, int newIndex);
    void onSongRemoved(int index);
};

#endif // PLAYLISTMANAGER_H
