#include "profileLoader.hpp"
#include "json_warn.hpp"
#include <QFile>
#include <QIODevice>
#include <QByteArray>

namespace mrst {
    LoadResult loadProfileFile(const QString& _path) {
        LoadResult result;

        QFile file(_path);
        if (!file.open(QIODevice::ReadOnly)) {
            warnUnreadable(file.errorString(),
                ParseContext{ "profile", WarningEffect::FileRejected, result.warnings });
            return result;
        }

        const QByteArray bytes = file.readAll();
        if (file.error() != QFileDevice::NoError) {
            warnUnreadable(file.errorString(),
                ParseContext{ "profile", WarningEffect::FileRejected, result.warnings });
            return result;
        }

        auto obj = parseProfileJson(bytes, result.warnings);
        if (!obj)
            return result;

        result.profile = parseProfile(obj.value(), result.warnings);
        
        return result;
    }
}