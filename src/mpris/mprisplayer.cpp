#include "mprisplayer.h"

#include <QDBusConnection>
#include <QDBusMessage>

MprisPlayer::MprisPlayer(PlaylistManager* playlist, QObject* parent)
    : QDBusAbstractAdaptor(parent)
    , playlist(playlist)
{
    connect(playlist, &PlaylistManager::playingChanged, this, [this]() {
        propertiesChanged(QVariantMap({{ QStringLiteral("PlaybackStatus"), PlaybackStatus() }}));
    });
    connect(playlist, &PlaylistManager::playbackPositionChanged, this, [this, playlist]() {
        emit Seeked(playlist->playbackPositionMs() * 1000); // to microseconds
        propertiesChanged(QVariantMap({{ QStringLiteral("Position"), Position() }}));
    });
    connect(playlist, &PlaylistManager::shuffleChanged, this, [this]() {
        propertiesChanged(QVariantMap({{ QStringLiteral("Shuffle"), Shuffle() }}));
    });
    connect(playlist, &PlaylistManager::songChanged, this, [this]() {
        QVariantMap properties;
        properties[QStringLiteral("PlaybackStatus")] = PlaybackStatus();
        properties[QStringLiteral("Metadata")] = Metadata();
        properties[QStringLiteral("Position")] = Position();
        properties[QStringLiteral("CanGoNext")] = CanGoNext();
        properties[QStringLiteral("CanGoPrevious")] = CanGoPrevious();
        properties[QStringLiteral("CanPlay")] = CanPlay();
        properties[QStringLiteral("CanPause")] = CanPause();
        properties[QStringLiteral("CanSeek")] = CanSeek();
        propertiesChanged(properties);
    });
}

QString MprisPlayer::PlaybackStatus() const {
    if (playlist->currentSong() == nullptr
            || (!playlist->isPlaying() && playlist->playbackPosition() == 0.0))
        return QStringLiteral("Stopped");

    if (playlist->isPlaying())
        return QStringLiteral("Playing");

    return QStringLiteral("Paused");
}

QString MprisPlayer::LoopStatus() const {
    return "None"; // looping is not implemented yet. Unless I forgot this after it got implemented
}

void MprisPlayer::setLoopStatus(QString loopStatus) {
    // again, not implemented yet unless I forgor
}

qreal MprisPlayer::Rate() const {
    return 1.0;
}

void MprisPlayer::setRate(qreal rate) {
    Q_UNUSED(rate)
}

bool MprisPlayer::Shuffle() const {
    return playlist->isShuffling();
}

void MprisPlayer::setShuffle(bool shuffle) {
    playlist->setShuffle(shuffle);
}

QVariantMap MprisPlayer::Metadata() const {
    if (playlist->currentSong() == nullptr)
        return {};

    QVariantMap metadata;
    metadata[QStringLiteral("mpris:trackid")]
        = QVariant::fromValue<QDBusObjectPath>(QDBusObjectPath(currentTrackID()));
    metadata[QStringLiteral("mpris:length")] = (qint64) playlist->currentSongDuration() * 1000; // to microseconds

    SongModel* song = playlist->currentSong();
    metadata[QStringLiteral("xesam:url")] = song->file.toString();
    metadata[QStringLiteral("xesam:title")] = song->title;
    metadata[QStringLiteral("xesam:artist")] = QStringList() << song->artist;
    metadata[QStringLiteral("mpris:artUrl")] = song->image.toString();
    return metadata;
}

qreal MprisPlayer::Volume() const {
    return 1.0;
}

void MprisPlayer::setVolume(qreal volume) {
    Q_UNUSED(volume);
}

qint64 MprisPlayer::Position() const {
    return playlist->playbackPositionMs() * 1000; // to microseconds
}

qreal MprisPlayer::MinimumRate() const {
    return 1.0;
}

qreal MprisPlayer::MaximumRate() const {
    return 1.0;
}

bool MprisPlayer::CanGoNext() const {
    return playlist->canPlayNext();
}

bool MprisPlayer::CanGoPrevious() const {
    return playlist->canPlayPrevious();
}

bool MprisPlayer::CanPlay() const {
    return playlist->currentSong() != nullptr;
}

bool MprisPlayer::CanPause() const {
    return playlist->currentSong() != nullptr;
}

bool MprisPlayer::CanSeek() const {
    return playlist->isSeekable();
}

bool MprisPlayer::CanControl() const {
    return true;
}

void MprisPlayer::Next() {
    if (CanGoNext())
        playlist->playNext();
}

void MprisPlayer::Previous() {
    if (CanGoPrevious())
        playlist->playPrevious();
}

void MprisPlayer::Pause() {
    if (CanPause())
        playlist->pause();
}

void MprisPlayer::PlayPause() {
    if (CanPause()) // I don't know why but the spec specifies this specifically
        playlist->togglePlay();
}

void MprisPlayer::Stop() {
    if (playlist->currentSong() != nullptr)
        return;

    playlist->stop();
}

void MprisPlayer::Play() {
    if (CanPlay())
        playlist->play();
}

void MprisPlayer::Seek(qint64 position) {
    if (!CanSeek())
        return;

    qint64 seek = position / 1000; // because x is in microseconds
    qint64 newPosition = playlist->playbackPosition() + seek;
    if (newPosition < 0) {
        playlist->setPlaybackPositionMs(0);
        return;
    }
    if (newPosition > playlist->currentSongDuration()) {
        playlist->playNext();
        return;
    }
    playlist->setPlaybackPositionMs(newPosition);
}

void MprisPlayer::SetPosition(const QDBusObjectPath& trackID, qint64 position) {
    if (!CanSeek() || trackID.path() != currentTrackID())
        return;

    playlist->setPlaybackPositionMs(position / 1000); // converting from microseconds
}

// TODO: implement opening uri from mpris
void MprisPlayer::OpenUri(const QString& uri) {
    Q_UNUSED(uri);
}

QString MprisPlayer::currentTrackID() const {
    return QStringLiteral("/me/doggirl/utaite/playlist/") + QString::number(playlist->currentSong()->playlistIndex);
}

void MprisPlayer::propertiesChanged(const QVariantMap& properties) {
    QDBusMessage signal = QDBusMessage::createSignal(
        QStringLiteral("/org/mpris/MediaPlayer2"),
        QStringLiteral("org.freedesktop.DBus.Properties"),
        QStringLiteral("PropertiesChanged"));

    signal << QStringLiteral("org.mpris.MediaPlayer2.Player");
    signal << properties;
    signal << QStringList();

    QDBusConnection::sessionBus().send(signal);
}
