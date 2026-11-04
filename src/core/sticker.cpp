#include "sticker.hpp"
#include "json_read.hpp"
#include "json_warn.hpp"
#include <algorithm>

namespace mrst {
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

        if (st.sizeMode == SizeMode::Size) {
            if (_obj.contains("width")) {
                checkRange(_obj, "width", ok, st.width, where, _warnings);
            }
            if (_obj.contains("height")) {
                checkRange(_obj, "height", ok, st.height, where, _warnings);
            }
        }
        else {
            if (_obj.contains("scale")) {
                checkRange(_obj, "scale", ok, st.scale, where, _warnings);
            }
        }

        ok = readDouble(_obj, "rotation", false, st.rotation, where, _warnings) and ok;

        if (_obj.contains("opacity")) {
            double clampedOpacity;
            ok = readDouble(_obj, "opacity", false, clampedOpacity, where, _warnings) and ok;
            if (clampedOpacity < 0 or clampedOpacity > 1) {
                st.opacity = std::clamp(clampedOpacity, 0.0, 1.0);
                warnClamped(where, "opacity", clampedOpacity, _warnings);
            }
        }

        if (!ok)
            return std::nullopt;
        return st;
    }
}