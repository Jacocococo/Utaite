#include "playlistmanager.h"

#include <QRandomGenerator>

PlaylistManager::PlaylistManager(QList<SongModel*>& songs, QObject* parent)
    : QObject(parent), songs(songs)
{
    player = new QMediaPlayer();
    audioOutput = new QAudioOutput();
    player->setAudioOutput(audioOutput);
    queueModel = new SongsEntryModel(queue);

    connect(player, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)),
            this, SLOT(onMediaStatusChanged(QMediaPlayer::MediaStatus)));
    connect(player, SIGNAL(positionChanged(qint64)),
            this, SLOT(onPositionChanged(qint64)));

    connect(queueModel, SIGNAL(songMoved(int, int)),
            this, SLOT(onSongMoved(int, int)));
    connect(queueModel, SIGNAL(songRemoved(int)),
            this, SLOT(onSongRemoved(int)));

    connect(this, SIGNAL(songChanged()), this, SLOT(onSongChanged()));
    connect(this, SIGNAL(queueChanged()), queueModel, SLOT(refresh()));
}

PlaylistManager::~PlaylistManager() {
    delete player;
}

void PlaylistManager::setSong(int index) {
    player->stop();
    queue.clear();
    queueCursor = 0;

    queue.append(songs[index]);
    for (int i = index + 1; i != index; i = (i + 1) % songs.size()) {
        queue.append(songs[i % songs.size()]);
    }

    if (shuffle)
        shuffleQueue();
    else // already run by shuffleQueue
        emit queueChanged();

    emit queueCursorChanged();
    emit songChanged();
}

void PlaylistManager::play() {
    player->play();
    playing = true;
    emit playingChanged();
}

void PlaylistManager::pause() {
    player->pause();
    playing = false;
    emit playingChanged();
}

void PlaylistManager::togglePlay() {
    if (playing)
        pause();
    else
        play();
}

void PlaylistManager::stop() {
    player->stop();
    playing = false;
    emit playingChanged();
}

void PlaylistManager::setQueueIndex(int index) {
    queueCursor = index;
    emit queueCursorChanged();
    emit songChanged();
}

bool PlaylistManager::canPlayNext() const {
    return queueCursor < queue.size() - 1;
}

bool PlaylistManager::canPlayPrevious() const {
    return queueCursor > 0;
}

void PlaylistManager::playNext() {
    if (canPlayNext())
        setQueueIndex(queueCursor + 1);
}

void PlaylistManager::playPrevious() {
    if (player->position() < 4000) { // TODO: I don't like the inline hardcoded value
        if (canPlayPrevious())
            setQueueIndex(queueCursor - 1);

        return;
    }

    player->setPosition(0);
}

void PlaylistManager::enqueue(int playlistIndex) {
    queue.append(songs[playlistIndex]);
    emit queueChanged();
}

void PlaylistManager::enqueueNext(int playlistIndex) {
    queue.insert(queueCursor + 1, songs[playlistIndex]);
    emit queueChanged();
}

void PlaylistManager::shuffleQueue() { // https://en.wikipedia.org/wiki/Fisher%E2%80%93Yates_shuffle#The_modern_algorithm
    for (int i = queueCursor + 1; i < queue.size() - 2; i++) {
        int j = QRandomGenerator::global()->bounded(queue.size() - i) + i;
        queue.swapItemsAt(i, j);
    }
    emit queueChanged();
}

void PlaylistManager::onMediaStatusChanged(QMediaPlayer::MediaStatus mediaStatus) {
    if (mediaStatus == QMediaPlayer::LoadedMedia) {
        if (playing)
            play();
    }

    if (mediaStatus == QMediaPlayer::EndOfMedia) {
        playNext();
    }
}

void PlaylistManager::onPositionChanged(qint64 pos) {
    if (player->duration() == 0)
        return;

    position = (qreal) pos / player->duration();
    emit playbackPositionChanged();
}

void PlaylistManager::onSongChanged() {
    if (!queue.empty()) {
        player->setSource(queue[queueCursor]->file);
    }
}

void PlaylistManager::onSongMoved(int oldIndex, int newIndex) {
    if (oldIndex == queueCursor) {
        queueCursor = newIndex;
        emit queueCursorChanged();
        return;
    }

    if (oldIndex > queueCursor && newIndex <= queueCursor) {
        queueCursor++;
        emit queueCursorChanged();
    } else if (oldIndex < queueCursor && newIndex >= queueCursor) {
        queueCursor--;
        emit queueCursorChanged();
    }
}

void PlaylistManager::onSongRemoved(int index) {
    if (index < queueCursor) {
        queueCursor--;
        emit queueCursorChanged();
        return;
    }

    if (queueCursor == index) {
        if (queueCursor >= queue.size())
            queueCursor = queue.size() - 1;

        emit songChanged();
    }
}

bool PlaylistManager::isPlaying() const {
    return playing;
}

bool PlaylistManager::isSeekable() const {
    return player->isSeekable();
}

bool PlaylistManager::isShuffling() const {
    return shuffle;
}

void PlaylistManager::setShuffle(bool shuffle) {
    this->shuffle = shuffle;
    emit shuffleChanged();
}

SongModel* PlaylistManager::currentSong() const {
    if (queue.empty())
        return nullptr;

    return queue[queueCursor];
}

QList<SongModel*> PlaylistManager::getQueue() const {
    return queue;
}

int PlaylistManager::getQueueCursor() const {
    return queueCursor;
}

SongsEntryModel* PlaylistManager::getQueueModel() const {
    return queueModel;
}

qreal PlaylistManager::playbackPosition() const {
    return position;
}

void PlaylistManager::setPlaybackPosition(qreal pos) {
    position = pos;
    player->setPosition(pos * player->duration());
}

qint64 PlaylistManager::currentSongDuration() const {
    return player->duration();
}

qint64 PlaylistManager::playbackPositionMs() const {
    return player->position();
}

void PlaylistManager::setPlaybackPositionMs(qint64 pos) {
    player->setPosition(pos);
}
