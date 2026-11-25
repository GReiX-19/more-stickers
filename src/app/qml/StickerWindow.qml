import QtQuick
import Quickshell
import Quickshell.Wayland

PanelWindow {
    id: win
    required property string modelData
    property var sticker

    Component.onCompleted: console.log("created:", modelData, "sticker is", sticker)

    anchors { top: true; left: true }
    margins { left: sticker.x; top: sticker.y }
    implicitWidth: sticker.width
    implicitHeight: sticker.height

    WlrLayershell.layer: WlrLayer.Overlay
    exclusionMode: ExclusionMode.Ignore
    color: "transparent"
    mask: Region {}

    AnimatedImage {
        anchors.fill: parent
        source: "file://" + win.sticker.path
        opacity: win.sticker.opacity
        playing: true
        cache: false
    }
}