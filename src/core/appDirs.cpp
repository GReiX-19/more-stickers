#include "appDirs.hpp"
#include <QDir>

namespace mrst {
    bool ensureAppDirs(const AppPaths& _paths, QString* _error) {
        const auto pDir = _paths.profilesDir();
        const auto stDir = _paths.stickersDir();
        if (!QDir().mkpath(pDir)) {
            if (_error) *_error = "cannot create " + pDir;
            return false;
        }
        if (!QDir().mkpath(stDir)) {
            if (_error) *_error = "cannot create " + stDir;
            return false;
        }
        return true;
    }
} // namespace mrst