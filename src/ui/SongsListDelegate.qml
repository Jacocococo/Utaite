import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls
import org.kde.kirigami as Kirigami

import Utaite

Kirigami.AbstractCard {
    id: entryDelegate

    showClickFeedback: true
    onClicked: {
        Playlist.setSong(model.playlistIndex)
        Playlist.play();
    }

    contentItem: Item {
        implicitWidth: delegateLayout.implicitWidth
        implicitHeight: delegateLayout.implicitHeight

        GridLayout {
            id: delegateLayout
            anchors {
                left: parent.left
                top: parent.top
                right: parent.right
            }
            columnSpacing: Kirigami.Units.largeSpacing
            columns: 3

            RoundedImage {
                source: model.image
                Layout.fillHeight: true
                Layout.maximumHeight: Kirigami.Units.iconSizes.huge
                Layout.preferredWidth: height
                radius: 8
                asynchronous: true
                fillMode: Image.PreserveAspectCrop
                clip: true
                mipmap: true
            }

            Kirigami.Heading {
                Layout.fillWidth: true
                text: model.title
                elide: Text.ElideRight
            }

            Controls.ToolButton {
                id: actionsMenuButton
                Layout.alignment: Qt.AlignRight
                icon.name: "overflow-menu"
                checkable: true

                onToggled: function() {
                    if (checked)
                        actionsMenu.popup(this, 0, this.height)
                    else
                        actionsMenu.dismiss()
                }

                // TODO: Icons
                Controls.Menu {
                    id: actionsMenu

                    closePolicy: Controls.Popup.CloseOnEscape | Controls.Popup.CloseOnPressOutsideParent
                    onClosed: actionsMenuButton.checked = false

                    Controls.MenuItem {
                        text: i18nc("@action:inmenu add song to queue", "Add to queue")
                        onTriggered: {
                            Playlist.enqueue(model.playlistIndex)
                        }
                    }

                    Controls.MenuItem {
                        text: i18nc("@action:inmenu add song as the next one in the queue", "Play next")
                        onTriggered: {
                            Playlist.enqueueNext(model.playlistIndex)
                        }
                    }
                }
            }
        }
    }
}
