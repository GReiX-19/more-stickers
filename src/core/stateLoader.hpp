#pragma once

#include <QString>
#include <QList>
#include <optional>
#include "state.hpp"
#include "Warning.hpp"

namespace mrst {
    struct LoadStateResult {
        std::optional<State> state;
        QList<Warning> warnings;
    };

    LoadStateResult loadStateFile(const QString& _path);
}