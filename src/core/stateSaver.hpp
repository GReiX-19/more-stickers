#pragma once

#include <QJsonObject>
#include <QString>

namespace mrst {
    struct State;

    QJsonObject stateToJson(const State& _state);
    bool saveStateFile(const QString& _path, const State& _state, QString* _error);
} // namespace mrst
