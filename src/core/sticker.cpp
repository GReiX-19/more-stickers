#include "sticker.hpp"

static QString typeToString(QJsonValue::Type _type) {
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
static std::optional<SizeMode> parseSizeMode(const QString& _mode) {
    if (_mode == "size")
        return SizeMode::Size;
    else if (_mode == "scale")
        return SizeMode::Scale;

    return std::nullopt;
}
static void warnMissing(const QString& _where, const QString& _key, QList<Warning>& _warnings) {
    _warnings.append(
        Warning{
            _where,
            { ": \"" + _key + "\" is a required value - sticker skipped" },
            Warning::Kind::Missing
        }
    );
}
static void warnWrongType(const QString& _where, const QString& _key, const QString& _expected,
    const QJsonValue& _val, QList<Warning>& _warnings) {
    _warnings.append(
        Warning{
            _where,
            { ": \"" + _key + "\" expected " + _expected + ", found " + typeToString(_val.type()) + " \"" +
            _val.toVariant().toString() + "\" - sticker skipped" },
            Warning::Kind::WrongType
        }
    );
}
static void warnEmpty(const QString& _where, const QString& _key, QList<Warning>& _warnings) {
    _warnings.append(
        Warning{
            _where,
            { ": \"" + _key + "\" must not be empty - sticker skipped" },
            Warning::Kind::Empty
        }
    );
}
static void warnInvalidValue(const QString& _where, const QString& _key, const QList<QString>& _allowedValues,
    QList<Warning>& _warnings) {
    QString alvals;
    for (const auto val : _allowedValues)
        alvals.append(val + " ");
    _warnings.append(
        Warning{
            _where,
            { ": \"" + _key + "\" must be only { " + alvals + "} - sticker skipped" },
            Warning::Kind::InvalidValue
        }
    );
}
static std::optional<bool> isContainedKey(const QJsonObject& _obj, const QString& _key, bool _required,
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

static bool readString(const QJsonObject& _obj, const QString& _key, bool _required, bool _nonEmpty,
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
static bool readDouble(const QJsonObject& _obj, const QString& _key, bool _required,
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
static bool readBool(const QJsonObject& _obj, const QString& _key, bool _required,
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

std::optional<Sticker> parseSticker(const QJsonObject& _obj, int _index, QList<Warning>& _warnings) {
    Sticker st{};
    st.raw = _obj;
    QString where = "stickers[" + QString::number(_index) + "]";
    bool ok = true;

    ok = readString(_obj, "id", true, true, st.id, where, _warnings) and ok;
    if (ok) where += " (id: \"" + st.id + "\")";

    ok = readString(_obj, "source", true, true, st.source, where, _warnings) and ok;
    ok = readBool(_obj, "enabled", false, st.enabled, where, _warnings) and ok;

    QString monitorName;
    ok = readString(_obj, "monitor", false, false, monitorName, where, _warnings) and ok;
    if (!monitorName.isEmpty())
        st.monitor.emplace(monitorName);

    ok = readDouble(_obj, "x", false, st.x, where, _warnings) and ok;
    ok = readDouble(_obj, "y", false, st.y, where, _warnings) and ok;

    if (_obj.contains("sizeMode")) {
        QString mode;
        if (!readString(_obj, "sizeMode", false, false, mode, where, _warnings)) {
            ok = false;
        }
        else if (auto parsed = parseSizeMode(mode)) {
            st.sizeMode = parsed.value();
        }
        else {
            warnInvalidValue(where, "sizeMode", { "size", "scale" }, _warnings);
            ok = false;
        }
    }

    // if (st.sizeMode == SizeMode::Size) {
    //     ok = readDouble(_obj, "width", false, st.width, where, _warnings) and ok;
    //     ok = readDouble(_obj, "height", false, st.height, where, _warnings) and ok;
    // }
    // else {
    //     ok = readDouble(_obj, "scale", false, st.scale, where, _warnings) and ok;
    // }

    // ok = readDouble(_obj, "rotation", false, st.rotation, where, _warnings) and ok;
    // ok = readDouble(_obj, "opacity", false, st.opacity, where, _warnings) and ok;

    if (!ok)
        return std::nullopt;
    return st;
}