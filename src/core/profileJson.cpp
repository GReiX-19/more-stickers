#include "profileJson.hpp"
#include "Warning.hpp"
#include "json_read.hpp"
#include "json_warn.hpp"
#include <QJsonArray>
#include <QJsonDocument>
#include <QSet>
#include <QJsonParseError>

namespace mrst {
    std::optional<QJsonObject> parseProfileJson(const QByteArray& _json, QList<Warning>& _warnings) {
        QJsonParseError err;
        const auto doc = QJsonDocument::fromJson(_json, &err);
        if (err.error != QJsonParseError::NoError) {
            warnInvalidJson(err.errorString(), err.offset,
                ParseContext{ "profile", WarningEffect::FileRejected, _warnings });
            return std::nullopt;
        }
        if (!doc.isObject()) {
            warnWrongType("", "object",
                doc.isArray() ? QJsonValue{ doc.array() } : QJsonValue{},
                ParseContext{ "profile", WarningEffect::FileRejected, _warnings });
            return std::nullopt;
        }

        return doc.object();
    }
    std::optional<Profile> parseProfile(const QJsonObject& _obj, QList<Warning>& _warnings) {
        Profile prof{};
        prof.raw = _obj;
        ParseContext ctx{ "profile", WarningEffect::FileRejected, _warnings };

        int version = 0;
        if (!readInt(_obj, "schemaVersion", FieldOption::Required, version, ctx)) {
            return std::nullopt;
        }
        if (version != 1) {
            warnInvalidValue("schemaVersion", { "1" }, ctx);
            return std::nullopt;
        }
        prof.schemaVersion = version;

        if (!readString(_obj, "name", FieldOptions{}, prof.name, ctx))
            return std::nullopt;

        if (!_obj.contains("stickers")) {
            warnMissing("stickers", ctx);
            return std::nullopt;
        }
        const auto val = _obj.value("stickers");
        if (!val.isArray()) {
            warnWrongType("stickers", "array", val, ctx);
            return std::nullopt;
        }

        const QJsonArray arr = val.toArray();
        QSet<QString> seen;
        for (qsizetype i = 0; i < arr.size(); ++i) {
            const ParseContext stCtx{ "stickers[" + QString::number(i) + "]", WarningEffect::StickerSkipped, _warnings };
            const auto elem = arr[i];
            if (!elem.isObject()) {
                warnWrongType("", "object", elem, stCtx);
                continue;
            }
            auto st = parseSticker(elem.toObject(), i, _warnings);
            if (!st)
                continue;
            if (seen.contains(st->id)) {
                warnDuplicate("id", stCtx);
                continue;
            }
            seen.insert(st->id);
            prof.stickers.append(std::move(st.value()));
        }

        return prof;
    }
}