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

    WlrLayershell.layer: WlrLayer.Overlay
    exclusionMode: ExclusionMode.Ignore
    color: "transparent"
    mask: Region {}

    AnimatedImage {
        id: img
        anchors.fill: parent
        source: win.sticker.url
        opacity: win.sticker.opacity
        playing: true
        cache: false
        onStatusChanged: if (status === Image.Error) console.warn("sticker", win.modelData, "failed to load:", win.sticker.url)
    }

    readonly property bool scaled: sticker.sizeMode === "scale"
    readonly property bool ready: img.status !== Image.Error && (!scaled || img.status === Image.Ready)

    visible: ready
    implicitWidth: scaled ? img.implicitWidth * sticker.scale : sticker.width
    implicitHeight: scaled ? img.implicitHeight * sticker.scale : sticker.height
}