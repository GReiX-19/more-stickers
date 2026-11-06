#include "json_read.hpp"
#include "json_warn.hpp"
#include "Warning.hpp"

namespace mrst {
    namespace {
        std::optional<bool> isContainedKey(const QJsonObject& _obj, const QString& _key, FieldOptions _options,
            const ParseContext _ctx) {
            if (!_obj.contains(_key)) {
                if (_options.testFlag(FieldOption::Required)) {
                    warnMissing(_ctx.where, _key, _ctx.effect, _ctx.warnings);
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
            warnWrongType(_ctx.where, _key, "string", val, _ctx.effect, _ctx.warnings);
            return false;
        }
        auto out = val.toString();
        if (_options.testFlag(FieldOption::NonEmpty) and out.isEmpty()) {
            warnEmpty(_ctx.where, _key, _ctx.effect, _ctx.warnings);
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
            warnWrongType(_ctx.where, _key, "number", val, _ctx.effect, _ctx.warnings);
            return false;
        }
        auto out = val.toDouble();
        if (_options.testFlag(FieldOption::Positive) and (out <= 0)) {
            warnOutOfRange(_ctx.where, _key, WarningEffect::StickerSkipped, _ctx.warnings);
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
            warnWrongType(_ctx.where, _key, "bool", val, WarningEffect::StickerSkipped, _ctx.warnings);
            return false;
        }
        _out = val.toBool();

        return true;
    }
}