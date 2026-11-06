#pragma once

#include <QJsonObject>
#include <QList>
#include <QString>

namespace mrst {
    struct Warning;
    enum class WarningEffect;

    void warnMissing(const QString& _where, const QString& _key, WarningEffect _effect, QList<Warning>& _warnings);
    void warnWrongType(const QString& _where, const QString& _key, const QString& _expected,
        const QJsonValue& _val, WarningEffect _effect, QList<Warning>& _warnings);
    void warnEmpty(const QString& _where, const QString& _key, WarningEffect _effect, QList<Warning>& _warnings);
    void warnInvalidValue(const QString& _where, const QString& _key, const QList<QString>& _allowedValues,
        WarningEffect _effect, QList<Warning>& _warnings);
    void warnOutOfRange(const QString& _where, const QString& _key, WarningEffect _effect, QList<Warning>& _warnings);
    void warnClamped(const QString& _where, const QString& _key, const double& _val, WarningEffect _effect,
        QList<Warning>& _warnings);
}