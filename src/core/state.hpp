#pragma once

#include <QJsonObject>
#include <QList>
#include <QString>
#include <optional>

namespace mrst {
    struct Warning;

    struct State {
        int schemaVersion = 0;
        QString activeProfile;
    };

    std::optional<State> parseState(const QJsonObject& _obj, QList<Warning>& _warnings);
}