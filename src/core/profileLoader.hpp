#pragma once

#include <QList>
#include <QString>
#include <optional>
#include "profileJson.hpp"
#include "Warning.hpp"

namespace mrst {
    struct LoadResult {
        std::optional<Profile> profile;
        QList<Warning> warnings;
    };

    LoadResult loadProfileFile(const QString& _path);
}