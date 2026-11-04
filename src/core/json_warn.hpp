#pragma once

class QString;
template<typename T>
class QList;
class QJsonValue;

namespace mrst {
    class Warning;

    void warnMissing(const QString& _where, const QString& _key, QList<Warning>& _warnings);
    void warnWrongType(const QString& _where, const QString& _key, const QString& _expected,
        const QJsonValue& _val, QList<Warning>& _warnings);
    void warnEmpty(const QString& _where, const QString& _key, QList<Warning>& _warnings);
    void warnInvalidValue(const QString& _where, const QString& _key, const QList<QString>& _allowedValues,
        QList<Warning>& _warnings);
    void warnOutOfRange(const QString& _where, const QString& _key, QList<Warning>& _warnings);
    void warnClamped(const QString& _where, const QString& _key, const double& _val, QList<Warning>& _warnings);
}