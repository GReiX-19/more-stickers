#pragma once

#include <QJsonObject>
#include <QByteArray>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonArray>
#include <optional>
#include "json_warn.hpp"

namespace mrst {
    inline std::optional<QJsonObject> parseJsonObject(const QByteArray& _json, const ParseContext& _ctx) {
        QJsonParseError err;
        const auto doc = QJsonDocument::fromJson(_json, &err);
        if (err.error != QJsonParseError::NoError) {
            warnInvalidJson(err.errorString(), err.offset, _ctx);
            return std::nullopt;
        }
        if (!doc.isObject()) {
            warnWrongType("", "object",
                doc.isArray() ? QJsonValue{ doc.array() } : QJsonValue{}, _ctx);
            return std::nullopt;
        }

        return doc.object();
    }
} // namespace mrst