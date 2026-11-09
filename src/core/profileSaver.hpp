#pragma once

#include <QJsonObject>
#include <QString>

namespace mrst {
    struct Sticker;
    struct Profile;

    QJsonObject stickerToJson(const Sticker& _st);
    QJsonObject profileToJson(const Profile& _prof);
    bool saveProfileFile(const QString& _path, const Profile& _prof, QString* _error);
} // namespace mrst