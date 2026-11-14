#include "state.hpp"
#include "json_read.hpp"
#include "json_warn.hpp"
#include "Warning.hpp"
#include "jsonObjectParser.hpp"

namespace mrst {
    bool isPlainFileName(const QString& _name) {
        return !_name.isEmpty()
            and !_name.contains(QLatin1Char('/'))
            and _name != "."
            and _name != "..";
    }
    QString validateState(const State& _state) {
        QStringList problems;

        if (_state.schemaVersion != 1)
            problems << "schemaVersion must be 1";
        if (!isPlainFileName(_state.activeProfile))
            problems << "activeProfile must be non-empty plain file name (no '/', not '.' or '..')";

        return problems.join('\n');
    }

    std::optional<QJsonObject> parseStateJson(const QByteArray& _json, QList<Warning>& _warnings) {
        return parseJsonObject(_json, ParseContext{ "state", WarningEffect::FileRejected, _warnings });
    }
    std::optional<State> parseState(const QJsonObject& _obj, QList<Warning>& _warnings) {
        State state{};
        ParseContext ctx{ "state", WarningEffect::FileRejected, _warnings };

        int version = 0;
        if (!readInt(_obj, "schemaVersion", FieldOption::Required, version, ctx)) {
            return std::nullopt;
        }
        if (version != 1) {
            warnInvalidValue("schemaVersion", { "1" }, ctx);
            return std::nullopt;
        }
        state.schemaVersion = version;

        QString profile;
        if (!readString(_obj, "activeProfile", FieldOption::Required | FieldOption::NonEmpty, profile, ctx)) {
            return std::nullopt;
        }
        if (!isPlainFileName(profile)) {
            warnInvalidFileName("activeProfile", ctx);
            return std::nullopt;
        }

        state.activeProfile = profile;

        return state;
    }
} // namespace mrst