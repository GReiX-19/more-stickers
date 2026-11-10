#pragma once

#include <QString>

namespace mrst {
    struct AppPaths {
        QString configDir;
        QString dataDir;

        QString profilesDir() const;
        QString stateFile() const;
        QString stickersDir() const;
    };

    AppPaths pathsUnder(const QString& _configBase, const QString& _dataBase);
    AppPaths defaultPaths();
} // namespace mrst;