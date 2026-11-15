#pragma once

#include <QList>
#include <QString>
#include <optional>
#include "Warning.hpp"

namespace mrst {
    struct AppPaths;

    struct ActiveProfile {
        std::optional<QString> path;
        QList<Warning> warnings;
    };

    ActiveProfile resolveActiveProfile(const AppPaths& _paths);
}