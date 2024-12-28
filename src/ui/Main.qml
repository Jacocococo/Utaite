import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls
import org.kde.kirigami as Kirigami

import Utaite

Kirigami.ApplicationWindow {
    id: root

    title: i18nc("@title:window", "Utaite")

    Component {
        id: currentlyPlayingPage
        CurrentlyPlayingPage {}
    }

    Connections {
        target: Playlist

        property bool pushedCurrentlyPlayingPage: false

        function onSongChanged() {
            if (!pushedCurrentlyPlayingPage) {
                root.pageStack.push(currentlyPlayingPage);
                pushedCurrentlyPlayingPage = true;
            } else {
                root.pageStack.goForward()
            }
        }
    }

    pageStack.initialPage: Kirigami.ScrollablePage {
        title: i18nc("@title:tab list of all songs", "Song list")

        Kirigami.CardsListView {
            id: songEntries
            model: songsModel
            delegate: SongsListDelegate {}
        }
    }

    Component {
        id: indexSheet

        IndexSheet {
            Component.onCompleted: open()
        }
    }

    Component.onCompleted: function() {
        if (!SongsIndex.mainPlaylistFileExists()) {
            indexSheet.createObject()
        }
    }
}
