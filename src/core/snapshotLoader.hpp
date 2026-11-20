#pragma once

#include "snapshot.hpp"
#include "Warning.hpp"
#include <QList>
#include <QString>
#include <optional>

namespace mrst {
    struct Snapshot;
    struct AppPaths;

    struct SnapshotResult {
        Snapshot snapshot;
        std::optional<QString> profilePath;
        QList<Warning> warnings;
    };

    SnapshotResult loadSnapshot(const AppPaths& _paths);
} // namespace mrs 
