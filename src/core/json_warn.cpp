#include "json_warn.hpp"
#include "Warning.hpp"

namespace mrst {
    namespace {
        QString typeToString(QJsonValue::Type _type) {
            switch (_type) {
            case QJsonValue::Type::Array: return QString{ "array" };
            case QJsonValue::Type::Bool: return QString{ "bool" };
            case QJsonValue::Type::Double: return QString{ "number" };
            case QJsonValue::Type::Null: return QString{ "null" };
            case QJsonValue::Type::Object: return QString{ "object" };
            case QJsonValue::Type::String: return QString{ "string" };
            case QJsonValue::Type::Undefined: return QString{ "undefined" };
            }
            return QString{};
        }
    }

    void warnMissing(const QString& _key, const ParseContext& _ctx) {
        _ctx.warnings.append(
            Warning{
                _ctx.where,
                _key,
                { ": \"" + _key + "\" is a required value" },
                Warning::Kind::Missing,
                _ctx.effect
            }
        );
    }
    void warnWrongType(const QString& _key, const QString& _expected, const QJsonValue& _val, const ParseContext& _ctx) {
        _ctx.warnings.append(
            Warning{
                _ctx.where,
                _key,
                { ": \"" + _key + "\" expected " + _expected + ", found " + typeToString(_val.type()) + " \"" +
                _val.toVariant().toString() + "\"" },
                Warning::Kind::WrongType,
                _ctx.effect
            }
        );
    }
    void warnEmpty(const QString& _key, const ParseContext& _ctx) {
        _ctx.warnings.append(
            Warning{
                _ctx.where,
                _key,
                { ": \"" + _key + "\" must not be empty" },
                Warning::Kind::Empty,
                _ctx.effect
            }
        );
    }
    void warnInvalidValue(const QString& _key, const QStringList& _allowedValues, const ParseContext& _ctx) {
        QString alvals = _allowedValues.join(", ");
        _ctx.warnings.append(
            Warning{
                _ctx.where,
                _key,
                { ": \"" + _key + "\" must be only { " + alvals + " }" },
                Warning::Kind::InvalidValue,
                _ctx.effect
            }
        );
    }
    void warnOutOfRange(const QString& _key, const ParseContext& _ctx) {
        _ctx.warnings.append(
            Warning{
                _ctx.where,
                _key,
                { ": \"" + _key + "\" must be greater than 0" },
                Warning::Kind::OutOfRange,
                _ctx.effect
            }
        );
    }
    void warnClamped(const QString& _key, const double& _val, const ParseContext& _ctx) {
        _ctx.warnings.append(
            Warning{
                _ctx.where,
                _key,
                { ": \"" + _key + "\" = " + QString::number(_val) + " was clamped" },
                Warning::Kind::Clamped,
                _ctx.effect
            }
        );
    }
}