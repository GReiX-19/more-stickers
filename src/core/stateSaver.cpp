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
        if (const QString problem = validateState(_state); !problem.isEmpty()) {
            if (_error) *_error = problem;
            return false;
        }

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