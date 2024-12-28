import QtQuick
import QtQuick.Effects

Image {
    id: image
    property real radius

    Rectangle {
        id: mask
        layer.enabled: true
        layer.smooth: true
        anchors.fill: parent
        radius: image.radius
        visible: false
    }

    layer.enabled: true
    layer.effect: MultiEffect {
        maskEnabled: true
        maskSource: mask
        maskThresholdMin: 0.5
        maskSpreadAtMin: 1.0
    }
}
