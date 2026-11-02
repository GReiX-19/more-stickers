#pragma once

#include <QJsonObject>
#include <QList>
#include <QString>
#include <optional>

enum class SizeMode {
    Size,
    Scale
};

struct Sticker {
    QString id;
    QString source;
    bool enabled = true;
    std::optional<QString> monitor;
    double x = 0, y = 0;
    SizeMode sizeMode = SizeMode::Size;
    double width = 200, height = 200;
    double scale = 1.0;
    double rotation = 0;
    double opacity = 1.0;
    QJsonObject raw;
};

struct Warning {
    QString where;
    QString message;
};

std::optional<Sticker> parseSticker(const QJsonObject& _obj, int _index, QList<Warning>& _warnings);