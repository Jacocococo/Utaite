import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls
import org.kde.kirigami as Kirigami

import Utaite

Kirigami.OverlayDrawer {
    edge: Qt.BottomEdge
    height: Math.min(parent.height * 0.75, queueList.contentHeight + 8)

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

        delegate: Controls.ItemDelegate {
            padding: 0
            width: ListView.view ? ListView.view.width : implicitWidth
            height: listItem.implicitHeight
            background: Item {}

            contentItem: Kirigami.SwipeListItem {
                id: listItem
                width: parent.width
                anchors {
                    left: parent.left
                    right: parent.right
                }

                backgroundColor: Playlist.queueCursor === model.index
                                   ? Kirigami.Theme.activeBackgroundColor
                                   : Kirigami.Theme.backgroundColor

                onClicked: {
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
                }

                actions: [
                    Kirigami.Action {
                        text: i18n("Remove from queue")
                        icon.name: "edit-delete-remove"
                        displayHint: Qt.ToolButtonIconOnly

                        onTriggered: {
                            queueList.model.remove(model.index)
                        }
                    }
                ]
            }
        }
    }
}
