import QtQuick
import Quickshell
import Quickshell.Io

Scope {
    id: root
    
    readonly property int protocolVersion: 1

    property string status: "none"
    property var stickers: []

    readonly property var index: {
        const order = []
        const map = {}
        for (const st of stickers){
            order.push(st.id)
            map[st.id] = st
        }
        return { order, map }
    }

    function applyLine(line) {
        let snap
        try {
            snap = JSON.parse(line)
        }
        catch (e) {
            console.warn("feed: unparsable line:", e)
            status = "protocol_error"
            stickers = []
            return
        }

        if (snap === null || typeof snap !== "object" || snap.v !== protocolVersion) {
            console.warn("feed: unsupported protocol version", snap.v)
            status = "protocol_error"
            stickers = []
            return
        }

        status = snap.status
        stickers = (snap.status === "ok" && Array.isArray(snap.stickers)) ? snap.stickers : []
        if (!healthyTimer.running)
            healthyTimer.start()
    }

    readonly property int restartBase: 500
    readonly property int restartMax: 5000
    property int restartDelay: restartBase

    Timer {
        id: restartTimer
        repeat: false
        onTriggered: feed.running = true
    }
    Timer {
        id: healthyTimer
        repeat: false
        interval: 2000
        onTriggered: restartDelay = restartBase
    }

    Process {
        id: feed
        command: ["/home/GReiX19/Projects/more-stickers/build/src/feed/sticker-feed"]
        running: true

        stdout: SplitParser {
            splitMarker: "\n"
            onRead: data => applyLine(data)
        }
        stderr: SplitParser {
            splitMarker: "\n"
            onRead: data => console.warn("feed stderr:", data)
        }

        property bool stoppedBySigterm: false

        onExited: (exitCode, exitStatus) => {
            console.warn("feed exited", exitCode, exitStatus)
            stoppedBySigterm = (exitCode === 15 && exitStatus === 1)
        }
        onRunningChanged: {
            if (running) return
            root.status = "feed_down"
            root.stickers = []
            healthyTimer.stop()
            console.warn("feed down, sigterm:", stoppedBySigterm)
            if (!stoppedBySigterm) {
                console.warn("feed restart in", root.restartDelay, "ms")
                restartTimer.interval = root.restartDelay
                restartTimer.start()
                root.restartDelay = Math.min(root.restartDelay * 2, root.restartMax)
            }
            stoppedBySigterm = false
        }
    }

    Variants {
        model: root.index.order
        delegate: StickerWindow { sticker: root.index.map[modelData] }
    }
}