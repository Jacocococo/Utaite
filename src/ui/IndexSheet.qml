import QtCore
import QtQuick
import QtQuick.Layouts
import QtQuick.Dialogs
import QtQuick.Controls as Controls
import org.kde.kirigami as Kirigami

import Utaite

Kirigami.OverlaySheet {
    id: root
    parent: applicationWindow().overlay

    width: parent.width * 0.8
    showCloseButton: false

    function close() {}

    OpacityAnimator on parent.opacity {
        id: exit
        from: 1
        to: 0
        duration: Kirigami.Units.longDuration
        running: false

        onFinished: root.destroy()
    }

    FolderDialog {
        id: folderDialog
        currentFolder: StandardPaths.standardLocations(StandardPaths.MusicLocation)[0]
        onAccepted: musicPath.text = selectedFolder.toString().replace("file://", "")
    }

    header: Kirigami.Heading {
        text: i18nc("@title:window library index", "Scan music library")
    }

    ColumnLayout {
        Controls.Label {
            text: i18n("Select music folder")
        }
        Row {
            Controls.TextField {
                id: musicPath
                placeholderText: "~/Music" // TODO: platform independent
            }

            Controls.Button {
                text: i18n("Open file picker")
                onClicked: folderDialog.open()
            }
        }

        Item {
            Layout.preferredHeight: Kirigami.Units.gridUnit
        }

        Controls.Button {
            text: i18nc("@action:button scan for music files in selected folder", "Scan selected folder")
            onClicked: function() {
                let path = musicPath.text
                if (!path)
                    path = StandardPaths.standardLocations(StandardPaths.MusicLocation)[0]

                // TODO: Show progress
                SongsIndex.indexFilesAndSaveMainPlaylist(path)

                exit.start()
            }
        }
    }
}
