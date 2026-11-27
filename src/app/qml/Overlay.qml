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
    
    function clearOnError() {
        status = "protocol_error"
        stickers = []
        return
    }
    function applyLine(line) {
        let snap
        try {
            snap = JSON.parse(line)
        }
        catch (e) {
            console.warn("feed: unparsable line:", e)
            return clearOnError()
        }

        if (snap === null || typeof snap !== "object") {
            console.warn("feed: snapshot is not an object or is null")
            return clearOnError()
        }

        if (snap.v !== protocolVersion) {
            console.warn("feed: unsupported protocol version", snap.v)
            return clearOnError()
        }

        status = snap.status
        stickers = (snap.status === "ok" && Array.isArray(snap.stickers)) ? snap.stickers : []
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
        interval: restartMax * 2
        onTriggered: restartDelay = restartBase
    }

    Process {
        id: feed
        command: ["sticker-feed"]
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
            if (running) {
                healthyTimer.start()
                return
            }
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