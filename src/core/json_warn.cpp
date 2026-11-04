#include "json_warn.hpp"
#include "json_read.hpp"
#include "sticker.hpp"

namespace mrst {
    void warnMissing(const QString& _where, const QString& _key, QList<Warning>& _warnings) {
        _warnings.append(
            Warning{
                _where,
                _key,
                { ": \"" + _key + "\" is a required value - sticker skipped" },
                Warning::Kind::Missing
            }
        );
    }
    void warnWrongType(const QString& _where, const QString& _key, const QString& _expected,
        const QJsonValue& _val, QList<Warning>& _warnings) {
        _warnings.append(
            Warning{
                _where,
                _key,
                { ": \"" + _key + "\" expected " + _expected + ", found " + typeToString(_val.type()) + " \"" +
                _val.toVariant().toString() + "\" - sticker skipped" },
                Warning::Kind::WrongType
            }
        );
    }
    void warnEmpty(const QString& _where, const QString& _key, QList<Warning>& _warnings) {
        _warnings.append(
            Warning{
                _where,
                _key,
                { ": \"" + _key + "\" must not be empty - sticker skipped" },
                Warning::Kind::Empty
            }
        );
    }
    void warnInvalidValue(const QString& _where, const QString& _key, const QList<QString>& _allowedValues,
        QList<Warning>& _warnings) {
        QString alvals;
        for (const auto val : _allowedValues)
            alvals.append(val + " ");
        _warnings.append(
            Warning{
                _where,
                _key,
                { ": \"" + _key + "\" must be only { " + alvals + "} - sticker skipped" },
                Warning::Kind::InvalidValue
            }
        );
    }
    void warnOutOfRange(const QString& _where, const QString& _key, QList<Warning>& _warnings) {
        _warnings.append(
            Warning{
                _where,
                _key,
                { ": \"" + _key + "\" is out of range - sticker skipped" },
                Warning::Kind::OutOfRange
            }
        );
    }
    void warnClamped(const QString& _where, const QString& _key, const double& _val, QList<Warning>& _warnings) {
        _warnings.append(
            Warning{
                _where,
                _key,
                { ": \"" + _key + "\" = " + QString::number(_val) + " was clamped (0..1)" },
                Warning::Kind::Clamped
            }
        );
    }
}