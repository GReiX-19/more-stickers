#pragma once

#include <QJsonObject>
#include <QList>
#include <QString>
#include <optional>
#include "sticker.hpp"

namespace mrst {
    struct Sticker;
    struct Warning;

    struct Profile {
        int schemaVersion = 0;
        QString name;
        QList<Sticker> stickers;
        QJsonObject raw;
    };

    std::optional<QJsonObject> parseProfileJson(const QByteArray& _json, QList<Warning>& _warnings);
    std::optional<Profile> parseProfile(const QJsonObject& _obj, QList<Warning>& _warnings);
}