#include "stateLoader.hpp"
#include "json_warn.hpp"
#include <QFile>
#include <QFileInfo>
#include <QIODevice>
#include <QByteArray>

namespace mrst {
    LoadStateResult loadStateFile(const QString& _path) {
        LoadStateResult result;

        QFile file(_path);
        if (!file.open(QIODevice::ReadOnly)) {
            if (!QFileInfo::exists(_path))
                return result;
            warnUnreadable(file.errorString(),
                ParseContext{ "state", WarningEffect::FileRejected, result.warnings });
            return result;
        }

        const QByteArray bytes = file.readAll();
        if (file.error() != QFileDevice::NoError) {
            warnUnreadable(file.errorString(),
                ParseContext{ "state", WarningEffect::FileRejected, result.warnings });
            return result;
        }

        auto obj = parseStateJson(bytes, result.warnings);
        if (!obj)
            return result;

        result.state = parseState(obj.value(), result.warnings);
        
        return result;
    }
} // namespace mrst