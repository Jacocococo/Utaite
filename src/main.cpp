#include <QApplication>
#include <QQmlApplicationEngine>
#include <QtQml>
#include <QQuickStyle>
#include <QtDBus>
#include <KLocalizedContext>
#include <KLocalizedString>

#include "songsindex.h"
#include "playlistmanager.h"
#include "songsentrymodel.h"
#include "mpris/mpris.h"
#include "mpris/mprisplayer.h"

int main(int argc, char *argv[]) {
    qputenv("QT_MEDIA_BACKEND", "ffmpeg"); // force ffmpeg as media backend
    QApplication app(argc, argv);
    KLocalizedString::setApplicationDomain("utaite");
    QApplication::setApplicationName(QStringLiteral("Utaite"));
    QApplication::setDesktopFileName(QStringLiteral("me.doggirl.utaite"));

    if (qEnvironmentVariableIsEmpty("QT_QUICK_CONTROLS_STYLE")) {
        QQuickStyle::setStyle(QStringLiteral("org.kde.desktop"));
    }

    SongsIndex* index = new SongsIndex();
    PlaylistManager* playlistManager = new PlaylistManager(index->songs);
    SongsEntryModel* songsModel = new SongsEntryModel(index->songs);

    // MPRIS
    bool dbusSuccessful = QDBusConnection::sessionBus().registerService(QStringLiteral("org.mpris.MediaPlayer2.utaite"));

    static QObject mprisController;
    Mpris* mpris;
    MprisPlayer* mprisPlayer;
    if (dbusSuccessful) {
        mpris = new Mpris(&mprisController);
        mprisPlayer = new MprisPlayer(playlistManager, &mprisController);
        QDBusConnection::sessionBus().registerObject(QStringLiteral("/org/mpris/MediaPlayer2"), &mprisController);
    }

    // TODO: I think it would be possible to make it update in real time. That would be cool
    QObject::connect(index, SIGNAL(updatedIndex()), songsModel, SLOT(refresh()));

    // TODO: Show application before this finishes and show progress
    index->readPlaylistFile(index->mainPlaylistFile);

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextObject(new KLocalizedContext(&engine));

    qmlRegisterSingletonInstance("Utaite", 1, 0, "Playlist", playlistManager);
    qmlRegisterSingletonInstance("Utaite", 1, 0, "SongsIndex", index);
    engine.rootContext()->setContextProperty("songsModel", songsModel);

    engine.load(QUrl(QStringLiteral("qrc:/Main.qml")));
    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    QObject* rootObject = engine.rootObjects().at(0);
    QObject::connect(mpris, &Mpris::raise, rootObject, [rootObject]() {
        QMetaObject::invokeMethod(rootObject, "raise");
    });

    int ret = app.exec();

    delete songsModel;
    delete playlistManager;
    delete index;

    return ret;
}
