#include "snapshotEncoder.hpp"
#include <QString>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

namespace mrst {
    namespace {
        QString snapStatusToString(Snapshot::Status _snapStatus) {
            switch (_snapStatus) {
            case Snapshot::Status::NoActiveProfile: return QString{ "no_active_profile" };
            case Snapshot::Status::StateError: return QString{ "state_error" };
            case Snapshot::Status::ProfileError: return QString{ "profile_error" };
            case Snapshot::Status::Ok: return QString{ "ok" };
            }
            return QString{};
        }
        QJsonObject snapStickerToJson(const SnapshotSticker& _st) {
            QJsonObject obj;
            obj["id"] = _st.id;
            obj["path"] = _st.path;
            if (_st.monitor)
                obj["monitor"] = *_st.monitor;
            obj["x"] = _st.x;
            obj["y"] = _st.y;
            obj["width"] = _st.width;
            obj["height"] = _st.height;
            obj["scale"] = _st.scale;
            obj["rotation"] = _st.rotation;
            obj["opacity"] = _st.opacity;
            obj["sizeMode"] = QString{ _st.sizeMode == SizeMode::Size ? "size" : "scale" };
            return obj;
        }
    }
    QByteArray encodeSnapshot(const Snapshot& _snap) {
        QJsonObject obj;
        obj["v"] = kProtocolVersion;
        obj["status"] = snapStatusToString(_snap.status);
        obj["profile"] = _snap.profileName;
        QJsonArray arr;
        for (const auto& st : _snap.stickers)
            arr.append(snapStickerToJson(st));
        obj["stickers"] = arr;

        return QJsonDocument(obj).toJson(QJsonDocument::Compact);
    }
} // namespace mrst