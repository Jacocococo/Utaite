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

        onVisibleChanged: function() {
            if (visible && Playlist.queueCursor > 0)
                positionViewAtIndex(Playlist.queueCursor, ListView.Beginning)
        }

        headerPositioning: ListView.OverlayHeader
        header: Kirigami.ListSectionHeader {
            text: i18n("Queue")
            width: parent.width
            z: 2
            background: Rectangle {
                anchors.fill: parent
                color: Kirigami.Theme.backgroundColor
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

                    Controls.ToolButton {
                        text: i18n("Remove from queue")
                        icon.name: "edit-delete-remove"
                        display: Controls.Button.IconOnly
                        flat: true

                        onClicked: function() {
                            queueList.model.remove(model.index)
                        }
                    }
                }
            }
        }
    }
}
