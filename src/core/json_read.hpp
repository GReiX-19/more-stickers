#pragma once

#include <QJsonObject>
#include <QList>
#include <QString>
#include <QFlags>

namespace mrst {
    struct Warning;

    enum class FieldOption {
        Required = 1 << 0,
        NonEmpty = 1 << 1,
        Positive = 1 << 2
    };
    Q_DECLARE_FLAGS(FieldOptions, FieldOption);
    Q_DECLARE_OPERATORS_FOR_FLAGS(FieldOptions);

    bool readString(const QJsonObject& _obj, const QString& _key, FieldOptions _options,
        QString& _out, const QString& _where, QList<Warning>& _warnings);
    bool readDouble(const QJsonObject& _obj, const QString& _key, FieldOptions _options,
        double& _out, const QString& _where, QList<Warning>& _warnings);
    bool readBool(const QJsonObject& _obj, const QString& _key, FieldOptions _options,
        bool& _out, const QString& _where, QList<Warning>& _warnings);
}