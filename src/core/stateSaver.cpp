#include "stateSaver.hpp"
#include "state.hpp"
#include <QSaveFile>
#include <QJsonDocument>
#include <QIODevice>

namespace mrst {
    QJsonObject stateToJson(const State& _state) {
        QJsonObject obj;
        obj["schemaVersion"] = _state.schemaVersion;
        obj["activeProfile"] = _state.activeProfile;
        return obj;
    }
    bool saveStateFile(const QString& _path, const State& _state, QString* _error) {
        QSaveFile f(_path);
        if (!f.open(QIODevice::WriteOnly)) {
            if (_error) *_error = f.errorString();
            return false;
        }

        f.write(QJsonDocument(stateToJson(_state)).toJson(QJsonDocument::Indented));
        if (!f.commit()) {
            if (_error) *_error = f.errorString();
            return false;
        }

        return true;
    }
} // namespace mrst