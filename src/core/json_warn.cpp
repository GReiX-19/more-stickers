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

    void warnMissing(const QString& _where, const QString& _key, WarningEffect _effect, QList<Warning>& _warnings) {
        _warnings.append(
            Warning{
                _where,
                _key,
                { ": \"" + _key + "\" is a required value" },
                Warning::Kind::Missing,
                _effect
            }
        );
    }
    void warnWrongType(const QString& _where, const QString& _key, const QString& _expected,
        const QJsonValue& _val, WarningEffect _effect, QList<Warning>& _warnings) {
        _warnings.append(
            Warning{
                _where,
                _key,
                { ": \"" + _key + "\" expected " + _expected + ", found " + typeToString(_val.type()) + " \"" +
                _val.toVariant().toString() + "\"" },
                Warning::Kind::WrongType,
                _effect
            }
        );
    }
    void warnEmpty(const QString& _where, const QString& _key, WarningEffect _effect, QList<Warning>& _warnings) {
        _warnings.append(
            Warning{
                _where,
                _key,
                { ": \"" + _key + "\" must not be empty" },
                Warning::Kind::Empty,
                _effect
            }
        );
    }
    void warnInvalidValue(const QString& _where, const QString& _key, const QStringList& _allowedValues,
        WarningEffect _effect, QList<Warning>& _warnings) {
        QString alvals = _allowedValues.join(", ");
        _warnings.append(
            Warning{
                _where,
                _key,
                { ": \"" + _key + "\" must be only { " + alvals + " }" },
                Warning::Kind::InvalidValue,
                _effect
            }
        );
    }
    void warnOutOfRange(const QString& _where, const QString& _key, WarningEffect _effect, QList<Warning>& _warnings) {
        _warnings.append(
            Warning{
                _where,
                _key,
                { ": \"" + _key + "\" must be greater than 0" },
                Warning::Kind::OutOfRange,
                _effect
            }
        );
    }
    void warnClamped(const QString& _where, const QString& _key, const double& _val, WarningEffect _effect,
        QList<Warning>& _warnings) {
        _warnings.append(
            Warning{
                _where,
                _key,
                { ": \"" + _key + "\" = " + QString::number(_val) + " was clamped" },
                Warning::Kind::Clamped,
                _effect
            }
        );
    }
}