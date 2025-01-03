import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls
import org.kde.kirigami as Kirigami

import Utaite

Kirigami.OverlayDrawer {
    edge: Qt.BottomEdge
    height: Math.min(parent?.height * 0.75 ?? 0, queueList.contentHeight + 8)

    contentItem: ListView {
        id: queueList
        clip: true

        model: Playlist.queueModel

        moveDisplaced: Transition {
            YAnimator {
                duration: Kirigami.Units.longDuration
                easing.type: Easing.InOutQuad
            }
        }

        delegate: Loader {
            width: queueList.width
            sourceComponent: Controls.ItemDelegate {
                id: listItem

                onClicked: function() {
                    Playlist.setQueueIndex(model.index)
                    close()
                }

                contentItem: RowLayout {
                    Kirigami.ListItemDragHandle {
                        listItem: listItem
                        listView: queueList
                        onMoveRequested: function(oldIndex, newIndex) {
                            queueList.model.move(oldIndex, newIndex)
                        }
                    }

                    RoundedImage {
                        source: model.image
                        Layout.fillHeight: true
                        Layout.maximumHeight: Kirigami.Units.iconSizes.large
                        Layout.preferredWidth: height
                        radius: 8
                        asynchronous: true
                        fillMode: Image.PreserveAspectCrop
                        clip: true
                        mipmap: true
                    }

                    Controls.Label {
                        Layout.fillWidth: true
                        text: model.title
                        elide: Text.ElideRight
                        color: Playlist.queueCursor <= model.index
                                 ? Kirigami.Theme.textColor
                                 : Kirigami.Theme.disabledTextColor
                    }

                    Kirigami.ActionToolBar {
                        Layout.fillWidth: false
                        display: Controls.Button.IconOnly
                        actions: [
                            Kirigami.Action {
                                text: i18n("Remove from queue")
                                icon.name: "edit-delete-remove"

                                onTriggered: {
                                    queueList.model.remove(model.index)
                                }
                            }
                        ]
                    }
                }
            }
        }
    }
}
