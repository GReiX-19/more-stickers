#include "json_read.hpp"
#include "json_warn.hpp"

namespace mrst {
    namespace {
        std::optional<bool> isContainedKey(const QJsonObject& _obj, const QString& _key, FieldOptions _options,
            const QString& _where, QList<Warning>& _warnings) {
            if (!_obj.contains(_key)) {
                if (_options.testFlag(FieldOption::Required)) {
                    warnMissing(_where, _key, _warnings);
                    return false;
                }
                else
                    return true;
            }
            return std::nullopt;
        }
    }

    bool readString(const QJsonObject& _obj, const QString& _key, FieldOptions _options,
        QString& _out, const QString& _where, QList<Warning>& _warnings) {
        if (auto isKey = isContainedKey(_obj, _key, _options, _where, _warnings))
            return isKey.value();

        auto val = _obj.value(_key);
        if (!val.isString()) {
            warnWrongType(_where, _key, "string", val, _warnings);
            return false;
        }
        auto out = val.toString();
        if (_options.testFlag(FieldOption::NonEmpty) and out.isEmpty()) {
            warnEmpty(_where, _key, _warnings);
            return false;
        }

        _out = out;
        return true;
    }
    bool readDouble(const QJsonObject& _obj, const QString& _key, FieldOptions _options,
        double& _out, const QString& _where, QList<Warning>& _warnings) {
        if (auto isKey = isContainedKey(_obj, _key, _options, _where, _warnings))
            return isKey.value();

        auto val = _obj.value(_key);
        if (!val.isDouble()) {
            warnWrongType(_where, _key, "number", val, _warnings);
            return false;
        }
        auto out = val.toDouble();
        if (_options.testFlag(FieldOption::Positive) and (out <= 0)) {
            warnOutOfRange(_where, _key, _warnings);
            return false;
        }

        _out = out;
        return true;
    }
    bool readBool(const QJsonObject& _obj, const QString& _key, FieldOptions _options,
        bool& _out, const QString& _where, QList<Warning>& _warnings) {
        if (auto isKey = isContainedKey(_obj, _key, _options, _where, _warnings))
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