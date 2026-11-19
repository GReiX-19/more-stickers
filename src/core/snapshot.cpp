#include "snapshot.hpp"
#include <QDir>

namespace mrst {
    namespace {
        QList<SnapshotSticker> createSnapshotStickersList(const QList<Sticker>& _stickers, const QString& _stickersDir) {
            QList<SnapshotSticker> snapStickers;
            const auto dir = QDir(_stickersDir);
            for (const auto& st : _stickers) {
                if (!st.enabled) continue;
                snapStickers.append(
                    SnapshotSticker{
                        .id = st.id,
                        .path = dir.filePath(st.source),
                        .monitor = st.monitor,
                        .x = st.x, .y = st.y,
                        .width = st.width, .height = st.height,
                        .scale = st.scale,
                        .rotation = st.rotation,
                        .opacity = st.opacity,
                        .sizeMode = st.sizeMode
                    }
                );
            }
            return snapStickers;
        }
    }
    Snapshot buildSnapshot(ActiveProfile::Status _active, const std::optional<Profile>& _profile,
        const QString& _stickersDir) {
        Snapshot snap;

        switch (_active) {
        case ActiveProfile::Status::NoState: snap.status = Snapshot::Status::NoActiveProfile; return snap;
        case ActiveProfile::Status::Rejected: snap.status = Snapshot::Status::StateError; return snap;
        case ActiveProfile::Status::Ready: {
            if (!_profile) {
                snap.status = Snapshot::Status::ProfileError;
                return snap;
            }
            snap.status = Snapshot::Status::Ok; break;
        }
        }

        snap.profileName = _profile->name;
        snap.stickers = createSnapshotStickersList(_profile->stickers, _stickersDir);

        return snap;
    }
} // namespace mrst