#pragma once

#include <QJsonObject>
#include <QList>
#include <QString>
#include <optional>

namespace mrst {
    struct Warning;
    enum class SizeMode;
    enum class FieldOption;
    typedef QFlags<FieldOption> FieldOptions;

    QString typeToString(QJsonValue::Type _type);
    std::optional<bool> isContainedKey(const QJsonObject& _obj, const QString& _key, FieldOptions _options,
        const QString& _where, QList<Warning>& _warnings);
    std::optional<SizeMode> parseSizeMode(const QString& _mode);

    bool readString(const QJsonObject& _obj, const QString& _key, FieldOptions _options,
        QString& _out, const QString& _where, QList<Warning>& _warnings);
    bool readDouble(const QJsonObject& _obj, const QString& _key, FieldOptions _options,
        double& _out, const QString& _where, QList<Warning>& _warnings);
    bool readBool(const QJsonObject& _obj, const QString& _key, FieldOptions _options,
        bool& _out, const QString& _where, QList<Warning>& _warnings);
}