#include "appPaths.hpp"
#include <QDir>
#include <QStandardPaths>

namespace mrst {
    QString AppPaths::profilesDir() const {
        return { configDir + "/profiles" };
    }
    QString AppPaths::stateFile() const {
        return { configDir + "/state.json" };
    }
    QString AppPaths::stickersDir() const {
        return { dataDir + "/stickers" };
    }

    AppPaths pathsUnder(const QString& _configBase, const QString& _dataBase) {
        const auto cbase = QDir::cleanPath(_configBase);
        const auto dbase = QDir::cleanPath(_dataBase);
        return AppPaths{ { cbase + "/more-stickers" },{ dbase + "/more-stickers" } };
    }
    AppPaths defaultPaths() {
        return pathsUnder(
            QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation),
            QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation)
        );
    }
} // namespace mrst