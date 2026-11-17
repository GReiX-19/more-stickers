#pragma once

#include <QList>
#include <QString>
#include <optional>
#include "Warning.hpp"

namespace mrst {
    struct AppPaths;

    struct ActiveProfile {
        enum class Status {
            NoState,
            Rejected,
            Ready
        };
        std::optional<QString> path;
        QList<Warning> warnings;
        Status status = Status::NoState;
    };

    bool rejectsFile(const QList<Warning>& _warnings);
    ActiveProfile resolveActiveProfile(const AppPaths& _paths);
}