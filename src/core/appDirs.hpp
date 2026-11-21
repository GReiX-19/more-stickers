#pragma once

#include <QString>
#include "appPaths.hpp"

namespace mrst {
    bool ensureAppDirs(const AppPaths& _paths, QString* _error);
} // namespace mrst