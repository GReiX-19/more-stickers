import QtQuick
import Quickshell
import Quickshell.Wayland

Scope {
    PanelWindow {
        id: stickerWindow
        anchors { 
            bottom: true
            right: true 
        }
        margins { right: 50 }

        implicitWidth: 320
        implicitHeight: 240

        WlrLayershell.layer: WlrLayer.Overlay
        exclusionMode: ExclusionMode.Ignore
        color: "transparent"

        mask: Region {}

        AnimatedImage {
            anchors.fill: parent
            source: "file:///home/GReiX19/Downloads/EffectCraft_projects/Teto/Teto Halloween Resized.webp"
            sourceSize: Qt.size(320, 240)
            playing: true
            cache: false
        }
    }
}