#include "json_read.hpp"
#include "json_warn.hpp"
#include "sticker.hpp"

namespace mrst {
    QString typeToString(QJsonValue::Type _type) {
        switch (_type) {
        case QJsonValue::Type::Array: return QString("array");
        case QJsonValue::Type::Bool: return QString("bool");
        case QJsonValue::Type::Double: return QString("number");
        case QJsonValue::Type::Null: return QString("null");
        case QJsonValue::Type::Object: return QString("object");
        case QJsonValue::Type::String: return QString("string");
        case QJsonValue::Type::Undefined: return QString("undefined");
        default: return QString();
        }
    }
    std::optional<bool> isContainedKey(const QJsonObject& _obj, const QString& _key, bool _required,
        const QString& _where, QList<Warning>& _warnings) {
        if (!_obj.contains(_key)) {
            if (_required) {
                warnMissing(_where, _key, _warnings);
                return false;
            }
            else
                return true;
        }
        return std::nullopt;
    }
    std::optional<SizeMode> parseSizeMode(const QString& _mode) {
        if (_mode == "size")
            return SizeMode::Size;
        else if (_mode == "scale")
            return SizeMode::Scale;

        return std::nullopt;
    }
    void checkRange(const QJsonObject& _obj, const QString& _key, bool& _ok,
        double& _out, const QString& _where, QList<Warning>& _warnings) {
        double val;
        _ok = readDouble(_obj, _key, false, val, _where, _warnings) and _ok;
        if (val < 0) {
            warnOutOfRange(_where, _key, _warnings);
            _ok = false;
        }
        else {
            _out = val;
        }
    }

    bool readString(const QJsonObject& _obj, const QString& _key, bool _required, bool _nonEmpty,
        QString& _out, const QString& _where, QList<Warning>& _warnings) {
        if (auto isKey = isContainedKey(_obj, _key, _required, _where, _warnings))
            return isKey.value();

        auto val = _obj.value(_key);
        if (!val.isString()) {
            warnWrongType(_where, _key, "string", val, _warnings);
            return false;
        }
        _out = val.toString();
        if (_nonEmpty and _out.isEmpty()) {
            warnEmpty(_where, _key, _warnings);
            return false;
        }

        return true;
    }
    bool readDouble(const QJsonObject& _obj, const QString& _key, bool _required,
        double& _out, const QString& _where, QList<Warning>& _warnings) {
        if (auto isKey = isContainedKey(_obj, _key, _required, _where, _warnings))
            return isKey.value();

        auto val = _obj.value(_key);
        if (!val.isDouble()) {
            warnWrongType(_where, _key, "number", val, _warnings);
            return false;
        }
        _out = val.toDouble();

        return true;
    }
    bool readBool(const QJsonObject& _obj, const QString& _key, bool _required,
        bool& _out, const QString& _where, QList<Warning>& _warnings) {
        if (auto isKey = isContainedKey(_obj, _key, _required, _where, _warnings))
            return isKey.value();

        auto val = _obj.value(_key);
        if (!val.isBool()) {
            warnWrongType(_where, _key, "bool", val, _warnings);
            return false;
        }
        _out = val.toBool();

        return true;
    }
}