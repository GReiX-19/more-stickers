#include "sticker.hpp"
#include "json_read.hpp"
#include "json_warn.hpp"
#include "Warning.hpp"
#include <algorithm>

namespace mrst {
    namespace {
        std::optional<SizeMode> parseSizeMode(const QString& _mode) {
            if (_mode == "size")
                return SizeMode::Size;
            else if (_mode == "scale")
                return SizeMode::Scale;

            return std::nullopt;
        }
    }

    std::optional<Sticker> parseSticker(const QJsonObject& _obj, int _index, QList<Warning>& _warnings) {
        Sticker st{};
        st.raw = _obj;
        QString where = "stickers[" + QString::number(_index) + "]";
        bool ok = true;

        ok = readString(_obj, "id", FieldOption::Required | FieldOption::NonEmpty, st.id, where, _warnings) and ok;
        if (ok) where += " (id: \"" + st.id + "\")";

        ok = readString(_obj, "source", FieldOption::Required | FieldOption::NonEmpty, st.source, where, _warnings) and ok;
        ok = readBool(_obj, "enabled", FieldOptions{}, st.enabled, where, _warnings) and ok;

        QString monitorName;
        ok = readString(_obj, "monitor", FieldOptions{}, monitorName, where, _warnings) and ok;
        if (!monitorName.isEmpty())
            st.monitor = monitorName;

        ok = readDouble(_obj, "x", FieldOptions{}, st.x, where, _warnings) and ok;
        ok = readDouble(_obj, "y", FieldOptions{}, st.y, where, _warnings) and ok;

        if (_obj.contains("sizeMode")) {
            QString mode;
            if (!readString(_obj, "sizeMode", FieldOptions{}, mode, where, _warnings)) {
                ok = false;
            }
            else if (auto parsed = parseSizeMode(mode)) {
                st.sizeMode = parsed.value();
            }
            else {
                warnInvalidValue(where, "sizeMode", { "size", "scale" }, WarningEffect::StickerSkipped, _warnings);
                ok = false;
            }
        }

        if (st.sizeMode == SizeMode::Size) {
            ok = readDouble(_obj, "width", FieldOption::Positive, st.width, where, _warnings) and ok;
            ok = readDouble(_obj, "height", FieldOption::Positive, st.height, where, _warnings) and ok;
        }
        else {
            ok = readDouble(_obj, "scale", FieldOption::Positive, st.scale, where, _warnings) and ok;
        }

        ok = readDouble(_obj, "rotation", FieldOptions{}, st.rotation, where, _warnings) and ok;

        if (_obj.contains("opacity")) {
            double opacity;
            if (!readDouble(_obj, "opacity", FieldOptions{}, opacity, where, _warnings)) {
                ok = false;
            }
            else if (opacity < 0 or opacity > 1) {
                st.opacity = std::clamp(opacity, 0.0, 1.0);
                warnClamped(where, "opacity", opacity, WarningEffect::ValueAdjusted, _warnings);
            }
            else {
                st.opacity = opacity;
            }
        }

        if (!ok)
            return std::nullopt;
        return st;
    }
}