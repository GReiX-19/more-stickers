#include "formatPaths.hpp"

namespace mrst {
    QString formatPaths(const AppPaths& _paths) {
        QString formatted;
        formatted += "Configs: " + _paths.configDir + "\n";
        formatted += "Data: " + _paths.dataDir + "\n";
        formatted += "Profiles: " + _paths.profilesDir() + "\n";
        formatted += "State file: " + _paths.stateFile() + "\n";
        formatted += "Stickers: " + _paths.stickersDir() + "\n";
        return formatted;
    }
} // namespace mrst