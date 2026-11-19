#pragma once

#include "snapshot.hpp"
#include "Warning.hpp"
#include <QList>

namespace mrst {
    struct Snapshot;
    struct AppPaths;

    struct SnapshotResult {
        Snapshot snapshot;
        QList<Warning> warnings;
    };

    SnapshotResult loadSnapshot(const AppPaths& _paths);
} // namespace mrs 
