#include "stateActions.hpp"
#include "appPaths.hpp"
#include "state.hpp"
#include "stateSaver.hpp"
#include <QFileInfo>

namespace mrst {
    std::optional<QString> setActiveProfile(const AppPaths& _paths, const QString& _name) {
        if (!isPlainFileName(_name))
            return QString{ _name + " must be non-empty plain file name (no '/', not '.' or '..')" };
        QFileInfo info(_paths.profilesDir() + '/' + _name);
        if (!info.exists())
            return QString{ info.filePath() + " does not exist" };
        if (!info.isFile())
            return QString{ info.filePath() + " must be a file" };

        State state{ 1, _name };
        QString error;
        if (!saveStateFile(_paths.stateFile(), state, &error))
            return error;

        return std::nullopt;
    }
}