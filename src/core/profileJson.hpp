#pragma once

#include <QJsonObject>
#include <QJsonValue>
#include <QList>
#include <QString>
#include <optional>
#include "sticker.hpp"

namespace mrst {
    struct Sticker;
    struct Warning;
    struct ParseContext;

    struct SkippedEntry {
        qsizetype index;
        QJsonValue value;
    };

    struct Profile {
        int schemaVersion = 0;
        QString name;
        QList<Sticker> stickers;
        QList<SkippedEntry> skipped;
        QJsonObject raw;
    };

    std::optional<QJsonObject> parseJsonObject(const QByteArray& _json, const ParseContext& _ctx);
    std::optional<QJsonObject> parseProfileJson(const QByteArray& _json, QList<Warning>& _warnings);
    std::optional<Profile> parseProfile(const QJsonObject& _obj, QList<Warning>& _warnings);
} //namespace mrst