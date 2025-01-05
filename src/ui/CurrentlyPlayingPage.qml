import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls
import org.kde.kirigami as Kirigami

import Utaite

Kirigami.Page {
    id: root

    readonly property var model: Playlist.currentSong

    title: model.title || "Current song"

    ColumnLayout {
        //anchors.centerIn: parent
        anchors {
            left: parent.left
            top: parent.top
            right: parent.right
        }
        spacing: Kirigami.Units.largeSpacing
        Layout.preferredHeight: parent.height
        Layout.topMargin: 100

        RoundedImage {
            Layout.alignment: Qt.AlignHCenter
            Layout.maximumHeight: 192
            Layout.preferredWidth: 192
            source: root.model.image
            radius: 16
            fillMode: Image.PreserveAspectCrop
            clip: true
            mipmap: true
        }

        Kirigami.Heading {
            Layout.alignment: Qt.AlignHCenter
            Layout.fillWidth: true
            horizontalAlignment: Text.AlignHCenter
            level: 1
            text: root.model.title
            font.weight: Font.Bold
            elide: Text.ElideRight
        }

        Kirigami.Heading {
            Layout.alignment: Qt.AlignHCenter
            Layout.fillWidth: true
            horizontalAlignment: Text.AlignHCenter
            level: 2
            text: root.model.artist
            elide: Text.ElideRight
        }

        Controls.Slider {
            Layout.alignment: Qt.AlignHCenter
            Layout.fillWidth: true
            Layout.maximumWidth: 256

            from: 0
            to: 1
            value: Playlist.playbackPosition

            onPressedChanged: {
                if (!pressed) {
                    Playlist.playbackPosition = position;
                }
            }
        }

        // TODO: size + shape?
        Row {
            Layout.alignment: Qt.AlignHCenter

            Controls.Button {
                id: prevButton
                icon.name: "media-skip-backward-symbolic"
                enabled: Playlist.canPlayPrevious

                onClicked: Playlist.playPrevious()
            }

            Controls.Button {
                id: playButton
                icon.name: Playlist.playing
                             ? "media-playback-pause-symbolic"
                             : "media-playback-start-symbolic"

                onClicked: Playlist.togglePlay()
            }

            Controls.Button {
                id: nextButton
                icon.name: "media-skip-forward-symbolic"
                enabled: Playlist.canPlayNext

                onClicked: Playlist.playNext()
            }
        }

        // TODO: place these buttons properly + i18n + icons
        Row {
            Layout.alignment: Qt.AlignHCenter

            Controls.Button {
                text: "Show queue"
                onClicked: {
                    queueDrawer.open()
                }
            }

            Controls.Button {
                text: "Re-shuffle"
                onClicked: {
                    Playlist.shuffleQueue();
                }
            }
        }
    }

    QueueDrawer {
        id: queueDrawer
        parent: root.parent
    }
}
