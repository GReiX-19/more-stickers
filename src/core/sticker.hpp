#pragma once

#include <QJsonObject>
#include <QList>
#include <QString>
#include <optional>

namespace mrst {
    enum class SizeMode {
        Size,
        Scale
    };

    struct Sticker {
        std::optional<QString> monitor;
        QString id;
        QString source;
        QJsonObject raw;
        double x = 0, y = 0;
        double width = 200, height = 200;
        double scale = 1.0;
        double rotation = 0;
        double opacity = 1.0;
        SizeMode sizeMode = SizeMode::Size;
        bool enabled = true;
    };

    struct Warning;

    std::optional<Sticker> parseSticker(const QJsonObject& _obj, qsizetype _index, QList<Warning>& _warnings);
} // namespace mrst