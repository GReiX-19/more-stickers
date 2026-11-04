#pragma once

#include <QJsonValue>

class QString;
template<typename T>
class QList;

namespace std {
    template<typename T>
    class optional;
}

namespace mrst {
    class Warning;
    enum class SizeMode;

    QString typeToString(QJsonValue::Type _type);
    std::optional<bool> isContainedKey(const QJsonObject& _obj, const QString& _key, bool _required,
        const QString& _where, QList<Warning>& _warnings);
    std::optional<SizeMode> parseSizeMode(const QString& _mode);
    void checkRange(const QJsonObject& _obj, const QString& _key, bool& _ok,
        double& _out, const QString& _where, QList<Warning>& _warnings);

    bool readString(const QJsonObject& _obj, const QString& _key, bool _required, bool _nonEmpty,
        QString& _out, const QString& _where, QList<Warning>& _warnings);
    bool readDouble(const QJsonObject& _obj, const QString& _key, bool _required,
        double& _out, const QString& _where, QList<Warning>& _warnings);
    bool readBool(const QJsonObject& _obj, const QString& _key, bool _required,
        bool& _out, const QString& _where, QList<Warning>& _warnings);
}