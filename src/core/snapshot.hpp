#pragma once

#include <QList>
#include <QString>
#include <optional>
#include <sticker.hpp>
#include <activeProfile.hpp>
#include <profileJson.hpp>

namespace mrst {
    struct SnapshotSticker {
        QString id;
        QString path;
        std::optional<QString> monitor;
        double x = 0, y = 0;
        double width = 200, height = 200;
        double scale = 1.0;
        double rotation = 0;
        double opacity = 1.0;
        SizeMode sizeMode = SizeMode::Size;

        bool operator==(const SnapshotSticker&) const = default;
    };

    struct Snapshot {
        enum class Status {
            NoActiveProfile,
            StateError,
            ProfileError,
            Ok
        };

        Status status = Status::NoActiveProfile;
        QString profileName;
        QList<SnapshotSticker> stickers;

        bool operator==(const Snapshot&) const = default;
    };

    Snapshot buildSnapshot(ActiveProfile::Status _active, const std::optional<Profile>& _profile,
        const QString& _stickersDir);
} // namespace mrst