#pragma once

#include <QJsonObject>
#include <QList>
#include <QString>
#include <QFlags>

namespace mrst {
    struct Warning;
    struct ParseContext;

    enum class FieldOption {
        Required = 1 << 0,
        NonEmpty = 1 << 1,
        Positive = 1 << 2
    };
    Q_DECLARE_FLAGS(FieldOptions, FieldOption)
    Q_DECLARE_OPERATORS_FOR_FLAGS(FieldOptions)

    bool readString(const QJsonObject& _obj, const QString& _key, FieldOptions _options,
        QString& _out, const ParseContext _ctx);
    bool readDouble(const QJsonObject& _obj, const QString& _key, FieldOptions _options,
        double& _out, const ParseContext _ctx);
    bool readBool(const QJsonObject& _obj, const QString& _key, FieldOptions _options,
        bool& _out, const ParseContext _ctx);
    bool readInt(const QJsonObject& _obj, const QString& _key, FieldOptions _options,
        int& _out, const ParseContext& _ctx);
}