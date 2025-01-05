#include "mpris.h"

#include <QApplication>

Mpris::Mpris(QObject* parent)
    : QDBusAbstractAdaptor(parent)
{}

bool Mpris::CanQuit() const {
    return true;
}

bool Mpris::Fullscreen() const {
    return false;
}

void Mpris::setFullscreen(bool fullscreen) {
    Q_UNUSED(fullscreen);
}

bool Mpris::CanSetFullscreen() const {
    return false;
}

bool Mpris::CanRaise() const {
    return true;
}

bool Mpris::HasTrackList() const {
    return false; // TODO: implement track list mpris
}

QString Mpris::Identity() const {
    return QApplication::applicationName();
}

QString Mpris::DesktopEntry() const {
    return QApplication::desktopFileName();
}

QStringList Mpris::SupportedUriSchemes() const {
    return QStringList() << QStringLiteral("file"); // TODO: other uri schemes are technically supported too
}

QStringList Mpris::SupportedMimeTypes() const {
    return QStringList(); // TODO: proper supported mime types list
}

void Mpris::Raise() {
    emit raise();
}

void Mpris::Quit() {
    QMetaObject::invokeMethod(QApplication::instance(), &QApplication::quit);
}
