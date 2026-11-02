#include "sticker.hpp"

static QString typeToString(QJsonValue::Type _type) {
    switch (_type) {
    case QJsonValue::Type::Array: return QString("array");
    case QJsonValue::Type::Bool: return QString("bool");
    case QJsonValue::Type::Double: return QString("double");
    case QJsonValue::Type::Null: return QString("null");
    case QJsonValue::Type::Object: return QString("object");
    case QJsonValue::Type::String: return QString("string");
    case QJsonValue::Type::Undefined: return QString("undefined");
    default: return QString();
    }
}

static bool readString(const QJsonObject& _obj, const QString& _key, bool _required,
    QString& _out, const QString& _where, QList<Warning>& _warnings) {
    if (!_obj.contains(_key)) {
        if (_required) {
            _warnings.append(Warning{ _where, "\"" + _key + "\" is a required value." });
            return false;
        }
        else
            return true;
    }

    auto val = _obj.value(_key);
    if (!val.isString()) {
        auto id = _obj.value("id");
        _warnings.append(Warning{ _where, "(id: \"" + id.toString() + "\"): \"" + _key +
            "\" expected string, found " + typeToString(val.type()) +
            " \"" + val.toVariant().toString() + "\" - sticker skipped" });
        return false;
    }
    _out = val.toString();

    return true;
}
static bool readDouble(const QJsonObject& _obj, const QString& _key, bool _required,
    double& _out, const QString& _where, QList<Warning>& _warnings) {
    if (!_obj.contains(_key)) {
        if (_required) {
            _warnings.append(Warning{ _where, "\"" + _key + "\" is a required value." });
            return false;
        }
        else
            return true;
    }

    auto val = _obj.value(_key);
    if (!val.isDouble()) {
        auto id = _obj.value("id");
        _warnings.append(Warning{ _where, "(id: \"" + id.toString() + "\"): \"" + _key +
            "\" expected number, found " + typeToString(val.type()) +
            " \"" + val.toVariant().toString() + "\" - sticker skipped" });
        return false;
    }
    _out = val.toDouble();

    return true;
}

std::optional<Sticker> parseSticker(const QJsonObject& _obj, int _index, QList<Warning>& _warnings) {
    Sticker st{};
    st.raw = _obj;
    QString where = "stickers[" + QString::number(_index) + "]";
    bool ok = true;

    ok = readString(_obj, "id", true, st.id, where, _warnings) and ok;
    ok = readString(_obj, "source", true, st.source, where, _warnings) and ok;
    ok = readDouble(_obj, "x", true, st.x, where, _warnings) and ok;
    ok = readDouble(_obj, "y", true, st.y, where, _warnings) and ok;

    if (!ok)
        return std::nullopt;
    return st;
}