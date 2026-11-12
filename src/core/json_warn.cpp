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
        QString valueSuffix(const QJsonValue& _val) {
            switch (_val.type()) {
                case QJsonValue::Type::String:
                case QJsonValue::Type::Double:
                case QJsonValue::Type::Bool:
                    return " \"" + _val.toVariant().toString() + "\"";
                default:
                    return {};
            }
        }
    }

    void warnMissing(const QString& _key, const ParseContext& _ctx) {
        _ctx.warnings.append(
            Warning{
                _ctx.where,
                _key,
                { "is a required value" },
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
                { "expected " + _expected + ", found " + typeToString(_val.type()) + valueSuffix(_val) },
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
                { "must not be empty" },
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
                { "must be only { " + alvals + " }" },
                Warning::Kind::InvalidValue,
                _ctx.effect
            }
        );
    }
    void warnOutOfRange(const QString& _key, const std::pair<QString, QString>& _ranges, const ParseContext& _ctx) {
        _ctx.warnings.append(
            Warning{
                _ctx.where,
                _key,
                { "is out of range, range ( " +  _ranges.first + ".." + _ranges.second + " )" },
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
                { "= " + QString::number(_val) + " was clamped" },
                Warning::Kind::Clamped,
                _ctx.effect
            }
        );
    }
    void warnDuplicate(const QString& _key, const ParseContext& _ctx) {
        _ctx.warnings.append(
            Warning{
                _ctx.where,
                _key,
                { "is a duplicate" },
                Warning::Kind::Duplicate,
                _ctx.effect
            }
        );
    }
    void warnInvalidJson(const QString& _errorText, const qsizetype _offset,
        const ParseContext& _ctx) {
        _ctx.warnings.append(
            Warning{
                _ctx.where,
                {},
                { _errorText + ", offset: " + QString::number(_offset) },
                Warning::Kind::InvalidJson,
                _ctx.effect
            }
        );
    }
    void warnUnreadable(const QString& _errorText, const ParseContext& _ctx) {
        _ctx.warnings.append(
            Warning{
                _ctx.where,
                {},
                { _errorText },
                Warning::Kind::Unreadable,
                _ctx.effect
            }
        );
    }
    void warnInvalidFileName(const QString& _key, const ParseContext& _ctx) {
        _ctx.warnings.append(
            Warning{
                _ctx.where,
                _key,
                { "must be a plain file name (no '/', not '.' or '..')" },
                Warning::Kind::InvalidValue,
                _ctx.effect
            }
        );
    }
} //namespace mrst