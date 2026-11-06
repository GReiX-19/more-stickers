#include "json_read.hpp"
#include "json_warn.hpp"
#include "Warning.hpp"
#include <cmath>
#include <limits>

namespace mrst {
    namespace {
        std::optional<bool> isContainedKey(const QJsonObject& _obj, const QString& _key, FieldOptions _options,
            const ParseContext _ctx) {
            if (!_obj.contains(_key)) {
                if (_options.testFlag(FieldOption::Required)) {
                    warnMissing(_key, _ctx);
                    return false;
                }
                else
                    return true;
            }
            return std::nullopt;
        }
    }

    bool readString(const QJsonObject& _obj, const QString& _key, FieldOptions _options,
        QString& _out, const ParseContext _ctx) {
        if (auto isKey = isContainedKey(_obj, _key, _options, _ctx))
            return isKey.value();

        auto val = _obj.value(_key);
        if (!val.isString()) {
            warnWrongType(_key, "string", val, _ctx);
            return false;
        }
        auto out = val.toString();
        if (_options.testFlag(FieldOption::NonEmpty) and out.isEmpty()) {
            warnEmpty(_key, _ctx);
            return false;
        }

        _out = out;
        return true;
    }
    bool readDouble(const QJsonObject& _obj, const QString& _key, FieldOptions _options,
        double& _out, const ParseContext _ctx) {
        if (auto isKey = isContainedKey(_obj, _key, _options, _ctx))
            return isKey.value();

        auto val = _obj.value(_key);
        if (!val.isDouble()) {
            warnWrongType(_key, "number", val, _ctx);
            return false;
        }
        auto out = val.toDouble();
        if (_options.testFlag(FieldOption::Positive) and (out <= 0)) {
            warnOutOfRange(_key, _ctx);
            return false;
        }

        _out = out;
        return true;
    }
    bool readBool(const QJsonObject& _obj, const QString& _key, FieldOptions _options,
        bool& _out, const ParseContext _ctx) {
        if (auto isKey = isContainedKey(_obj, _key, _options, _ctx))
            return isKey.value();

        auto val = _obj.value(_key);
        if (!val.isBool()) {
            warnWrongType( _key, "bool", val, _ctx);
            return false;
        }
        
        _out = val.toBool();
        return true;
    }
    bool readInt(const QJsonObject& _obj, const QString& _key, FieldOptions _options,
        int& _out, const ParseContext& _ctx) {
        if (auto isKey = isContainedKey(_obj, _key, _options, _ctx))
            return isKey.value();

        auto val = _obj.value(_key);
        if (!val.isDouble()) {
            warnWrongType(_key, "integer", val, _ctx);
            return false;
        }

        const double d = val.toDouble();
        const bool fitsInt = d >= std::numeric_limits<int>::min() and d <= std::numeric_limits<int>::max();

        if (!fitsInt or d != std::trunc(d)) {
            warnWrongType(_key, "integer", val, _ctx);
            return false;
        }

        _out = static_cast<int>(d);
        return true;
    }
}