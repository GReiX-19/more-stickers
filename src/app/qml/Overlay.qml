import QtQuick
import Quickshell
import Quickshell.Io

Scope {
    id: root
    
    readonly property int protocolVersion: 1

    property string status: "none"
    property var stickers: []

    readonly property var index: {
        const order = [];
        const map = {};
        for (const st of stickers){
            order.push(st.id);
            map[st.id] = st;
        }
        return { order, map };
    }

    function applyLine(line) {
        let snap;
        try {
            snap = JSON.parse(line);
        }
        catch (e) {
            console.warn("feed: unparsable line:", e);
            status = "protocol_error";
            stickers = [];
            return;
        }

        if (snap.v !== protocolVersion) {
            console.warn("feed: unsupported protocol version", snap.v);
            status = "protocol_error";
            stickers = [];
            return;
        }

        status = snap.status;
        stickers = (snap.status === "ok" && Array.isArray(snap.stickers)) ? snap.stickers : [];
        console.log("status:", status, "stickers:", stickers.length);
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

        onExited: (exitCode, exitStatus) => console.warn("feed exited", exitCode, exitStatus)
    }

    Variants {
        model: root.index.order
        delegate: StickerWindow { sticker: root.index.map[modelData] }
    }
}