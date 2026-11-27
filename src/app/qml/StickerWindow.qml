import QtQuick
import Quickshell
import Quickshell.Wayland

PanelWindow {
    id: win
    required property string modelData
    property var sticker

    readonly property var monitors: {
        const map = {}
        for (const scrn of Quickshell.screens) {
            map[scrn.name] = scrn
        }
        return map
    }
    readonly property var currentMonitor: {
        (sticker.monitor === "" || sticker.monitor === undefined) ? Quickshell.screens[0] : monitors[sticker.monitor]
    }

    readonly property bool scaled: sticker.sizeMode === "scale"
    readonly property bool ready: img.status !== Image.Error && (!scaled || img.status === Image.Ready)

    readonly property bool hasMonitor: !!currentMonitor
    visible: ready && hasMonitor

    readonly property real stW: scaled ? img.implicitWidth * sticker.scale : sticker.width
    readonly property real stH: scaled ? img.implicitHeight * sticker.scale : sticker.height

    readonly property real rad: sticker.rotation * Math.PI / 180
    readonly property real boxW: Math.ceil(Math.abs(stW * Math.cos(rad)) + Math.abs(stH * Math.sin(rad)))
    readonly property real boxH: Math.ceil(Math.abs(stW * Math.sin(rad)) + Math.abs(stH * Math.cos(rad)))

    implicitWidth: boxW
    implicitHeight: boxH
    margins {
        left: Math.round(sticker.x - (boxW - stW) / 2)
        top: Math.round(sticker.y - (boxH - stH) / 2)
    }

    screen: currentMonitor
    anchors { 
        top: true
        left: true 
    }
    WlrLayershell.layer: WlrLayer.Overlay
    exclusionMode: ExclusionMode.Ignore
    color: "transparent"
    mask: Region {}

    AnimatedImage {
        id: img
        source: win.sticker.url
        anchors.centerIn: parent
        width: win.stW
        height: win.stH
        opacity: win.sticker.opacity
        rotation: win.sticker.rotation
        playing: true
        cache: false
        onStatusChanged: if (status === Image.Error) console.warn("sticker", win.modelData, "failed to load:", win.sticker.url)
    }
}