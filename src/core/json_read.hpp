#pragma once

#include <QJsonValue>

class QString;
template<typename T>
class QList;
template<typename Enum>
class QFlags;

namespace std {
    template<typename T>
    class optional;
}

namespace mrst {
    class Warning;
    enum class SizeMode;
    enum class FieldOption;
    typedef QFlags<FieldOption> FieldOptions;

    QString typeToString(QJsonValue::Type _type);
    std::optional<bool> isContainedKey(const QJsonObject& _obj, const QString& _key, FieldOptions _options,
        const QString& _where, QList<Warning>& _warnings);
    std::optional<SizeMode> parseSizeMode(const QString& _mode);
    void checkRange(const QJsonObject& _obj, const QString& _key, bool& _ok,
        double& _out, const QString& _where, QList<Warning>& _warnings);

    bool readString(const QJsonObject& _obj, const QString& _key, FieldOptions _options,
        QString& _out, const QString& _where, QList<Warning>& _warnings);
    bool readDouble(const QJsonObject& _obj, const QString& _key, FieldOptions _options,
        double& _out, const QString& _where, QList<Warning>& _warnings);
    bool readBool(const QJsonObject& _obj, const QString& _key, FieldOptions _options,
        bool& _out, const QString& _where, QList<Warning>& _warnings);
}