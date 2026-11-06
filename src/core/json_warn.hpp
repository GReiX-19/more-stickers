#pragma once

#include <QJsonObject>
#include <QList>
#include <QString>

namespace mrst {
    struct Warning;
    struct ParseContext;
    enum class WarningEffect;

    void warnMissing(const QString& _key, const ParseContext& _ctx);
    void warnWrongType(const QString& _key, const QString& _expected, const QJsonValue& _val, const ParseContext& _ctx);
    void warnEmpty(const QString& _key, const ParseContext& _ctx);
    void warnInvalidValue(const QString& _key, const QList<QString>& _allowedValues, const ParseContext& _ctx);
    void warnOutOfRange(const QString& _key, const ParseContext& _ctx);
    void warnClamped(const QString& _key, const double& _val, const ParseContext& _ctx);
    void warnDuplicate();
}