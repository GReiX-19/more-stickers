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
        ParseContext ctx{ { "stickers[" + QString::number(_index) + "]" }, WarningEffect::StickerSkipped, _warnings };
        bool ok = true;

        ok = readString(_obj, "id", FieldOption::Required | FieldOption::NonEmpty, st.id, ctx) and ok;
        if (ok) ctx.where += " (id: \"" + st.id + "\")";

        ok = readString(_obj, "source", FieldOption::Required | FieldOption::NonEmpty, st.source, ctx) and ok;
        ok = readBool(_obj, "enabled", FieldOptions{}, st.enabled, ctx) and ok;

        QString monitorName;
        ok = readString(_obj, "monitor", FieldOptions{}, monitorName, ctx) and ok;
        if (!monitorName.isEmpty())
            st.monitor = monitorName;

        ok = readDouble(_obj, "x", FieldOptions{}, st.x, ctx) and ok;
        ok = readDouble(_obj, "y", FieldOptions{}, st.y, ctx) and ok;

        if (_obj.contains("sizeMode")) {
            QString mode;
            if (!readString(_obj, "sizeMode", FieldOptions{}, mode, ctx)) {
                ok = false;
            }
            else if (auto parsed = parseSizeMode(mode)) {
                st.sizeMode = parsed.value();
            }
            else {
                warnInvalidValue("sizeMode", { "size", "scale" }, ctx);
                ok = false;
            }
        }

        if (st.sizeMode == SizeMode::Size) {
            ok = readDouble(_obj, "width", FieldOption::Positive, st.width, ctx) and ok;
            ok = readDouble(_obj, "height", FieldOption::Positive, st.height, ctx) and ok;
        }
        else {
            ok = readDouble(_obj, "scale", FieldOption::Positive, st.scale, ctx) and ok;
        }

        ok = readDouble(_obj, "rotation", FieldOptions{}, st.rotation, ctx) and ok;

        if (_obj.contains("opacity")) {
            double opacity;
            if (!readDouble(_obj, "opacity", FieldOptions{}, opacity, ctx)) {
                ok = false;
            }
            else if (opacity < 0 or opacity > 1) {
                st.opacity = std::clamp(opacity, 0.0, 1.0);
                warnClamped("opacity", opacity, ParseContext{ ctx.where, WarningEffect::ValueAdjusted, _warnings });
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