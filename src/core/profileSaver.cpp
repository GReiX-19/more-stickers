#include "profileSaver.hpp"
#include "sticker.hpp"
#include "profileJson.hpp"
#include <QJsonArray>
#include <QSaveFile>
#include <QIODevice>
#include <QJsonDocument>

namespace mrst {
    namespace {
        template<class T>
        void put(QJsonObject& _obj, const QString& _key, const T& _val, const T& _def) {
            if (_obj.contains(_key) or _val != _def)
                _obj[_key] = _val;
        }
    }

    QJsonObject stickerToJson(const Sticker& _st) {
        QJsonObject obj = _st.raw;
        obj["id"] = _st.id;
        obj["source"] = _st.source;
        put(obj, "enabled", _st.enabled, true);
        if (_st.monitor)
            obj["monitor"] = _st.monitor.value();
        else if (!obj.value("monitor").toString().isEmpty())
            obj.remove("monitor");
        put(obj, "x", _st.x, 0.0);
        put(obj, "y", _st.y, 0.0);
        put(obj, "sizeMode", QString{ _st.sizeMode == SizeMode::Size ? "size" : "scale" }, QString{ "size" });
        if (_st.sizeMode == SizeMode::Size) {
            put(obj, "width", _st.width, 200.0);
            put(obj, "height", _st.height, 200.0);
        }
        else {
            put(obj, "scale", _st.scale, 1.0);
        }
        put(obj, "rotation", _st.rotation, 0.0);
        put(obj, "opacity", _st.opacity, 1.0);

        return obj;
    }
    QJsonObject profileToJson(const Profile& _prof) {
        QJsonObject obj = _prof.raw;
        obj["schemaVersion"] = _prof.schemaVersion;
        if (!_prof.name.isEmpty() or obj.contains("name"))
            obj["name"] = _prof.name;

        QJsonArray arr;
        for (const auto& st : _prof.stickers)
            arr.append(stickerToJson(st));
        for (const auto& sk : _prof.skipped)
            arr.insert(qMin<qsizetype>(sk.index, arr.size()), sk.value);
        obj["stickers"] = arr;

        return obj;
    }
    bool saveProfileFile(const QString& _path, const Profile& _prof, QString* _error) {
        QSaveFile f(_path);
        if (!f.open(QIODevice::WriteOnly)) {
            if (_error) *_error = f.errorString();
            return false;
        }

        f.write(QJsonDocument(profileToJson(_prof)).toJson(QJsonDocument::Indented));
        if (!f.commit()) {
            if (_error) *_error = f.errorString();
            return false;
        }

        return true;
    }
} // namespace mrst