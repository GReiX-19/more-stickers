#include "sticker.hpp"
#include "Warning.hpp"
#include <QtTest>

using namespace mrst;

class StickerTest : public QObject {
    Q_OBJECT;

private slots:
    void happyPath() {
        QJsonObject obj{ { "id", "a" },{ "source", "a.png" },{ "x", 123 },{ "y", 456 } };
        QList<Warning> lw;
        auto st = parseSticker(obj, 0, lw);

        QVERIFY(st.has_value());
        QCOMPARE(lw.size(), 0);
        QCOMPARE(st->id, "a");
        QCOMPARE(st->source, "a.png");
        QCOMPARE(st->x, 123);
        QCOMPARE(st->y, 456);
    }
    void missingRequiredField() {
        QJsonObject obj{ { "source", "a.png" } };
        QList<Warning> lw;
        auto st = parseSticker(obj, 0, lw);

        QCOMPARE(st.has_value(), false);
        QCOMPARE(lw.size(), 1);
        QCOMPARE(lw[0].key, "id");
        QCOMPARE(lw[0].kind, Warning::Kind::Missing);
        QCOMPARE(lw[0].effect, WarningEffect::StickerSkipped);
        QCOMPARE(lw[0].message, ": \"id\" is a required value");
    }
    void wrongType() {
        QJsonObject obj{ { "id", 123 },{ "source", "a.png" },{ "x", "456" } };
        QList<Warning> lw;
        auto st = parseSticker(obj, 0, lw);

        QCOMPARE(st.has_value(), false);
        QCOMPARE(lw.size(), 2);
        QCOMPARE(lw[0].key, "id");
        QCOMPARE(lw[0].kind, Warning::Kind::WrongType);
        QCOMPARE(lw[0].effect, WarningEffect::StickerSkipped);
        QCOMPARE(lw[1].key, "x");
        QCOMPARE(lw[1].kind, Warning::Kind::WrongType);
        QCOMPARE(lw[1].effect, WarningEffect::StickerSkipped);
    }
    void nonEmptyProps() {
        QJsonObject obj{ { "id", "" },{ "source", "" } };
        QList<Warning> lw;
        auto st = parseSticker(obj, 0, lw);

        QCOMPARE(st.has_value(), false);
        QCOMPARE(lw.size(), 2);
        QCOMPARE(lw[0].key, "id");
        QCOMPARE(lw[0].kind, Warning::Kind::Empty);
        QCOMPARE(lw[0].effect, WarningEffect::StickerSkipped);
        QCOMPARE(lw[1].key, "source");
        QCOMPARE(lw[1].kind, Warning::Kind::Empty);
        QCOMPARE(lw[1].effect, WarningEffect::StickerSkipped);
    }
    void optionalFields() {
        QJsonObject obj{ { "id", "a" },{ "source", "a.png" } };
        QList<Warning> lw;
        auto st = parseSticker(obj, 0, lw);

        QVERIFY(st.has_value());
        QCOMPARE(lw.size(), 0);
        QCOMPARE(st->id, "a");
        QCOMPARE(st->source, "a.png");
        QCOMPARE(st->enabled, true);
        QCOMPARE(st->monitor, std::nullopt);
        QCOMPARE(st->x, 0);
        QCOMPARE(st->y, 0);
        QCOMPARE(st->sizeMode, SizeMode::Size);
        QCOMPARE(st->width, 200);
        QCOMPARE(st->height, 200);
        QCOMPARE(st->scale, 1.0);
        QCOMPARE(st->rotation, 0);
        QCOMPARE(st->opacity, 1);
    }
    void enabledBoolCheck() {
        QJsonObject obj{ { "id", "a" },{ "source", "a.png" },{ "enabled", false } };
        QList<Warning> lw;
        auto st = parseSticker(obj, 0, lw);

        QVERIFY(st.has_value());
        QCOMPARE(lw.size(), 0);
        QCOMPARE(st->enabled, false);
    }
    void wrongTypeEnabled() {
        QJsonObject obj{ { "id", "a" },{ "source", "a.png" },{ "enabled", "yes" } };
        QList<Warning> lw;
        auto st = parseSticker(obj, 0, lw);

        QCOMPARE(st.has_value(), false);
        QCOMPARE(lw.size(), 1);
        QCOMPARE(lw[0].key, "enabled");
        QCOMPARE(lw[0].kind, Warning::Kind::WrongType);
        QCOMPARE(lw[0].effect, WarningEffect::StickerSkipped);
    }
    void emptyMonitorName() {
        QJsonObject obj{ { "id", "a" },{ "source", "a.png" },{ "monitor", "" } };
        QList<Warning> lw;
        auto st = parseSticker(obj, 0, lw);

        QVERIFY(st.has_value());
        QCOMPARE(lw.size(), 0);
        QCOMPARE(st->monitor, std::nullopt);
    }
    void wrongTypeMonitor() {
        QJsonObject obj{ { "id", "a" },{ "source", "a.png" },{ "monitor", 5 } };
        QList<Warning> lw;
        auto st = parseSticker(obj, 0, lw);

        QCOMPARE(st.has_value(), false);
        QCOMPARE(lw.size(), 1);
        QCOMPARE(lw[0].key, "monitor");
        QCOMPARE(lw[0].kind, Warning::Kind::WrongType);
        QCOMPARE(lw[0].effect, WarningEffect::StickerSkipped);
    }
    void rightTypeSizeMode() {
        QJsonObject obj{ { "id", "a" },{ "source", "a.png" },{ "sizeMode", "scale" } };
        QList<Warning> lw;
        auto st = parseSticker(obj, 0, lw);

        QVERIFY(st.has_value());
        QCOMPARE(lw.size(), 0);
        QCOMPARE(st->sizeMode, SizeMode::Scale);
    }
    void wrongTypeSizeMode() {
        QJsonObject obj{ { "id", "a" },{ "source", "a.png" },{ "sizeMode", 5 } };
        QList<Warning> lw;
        auto st = parseSticker(obj, 0, lw);

        QCOMPARE(st.has_value(), false);
        QCOMPARE(lw.size(), 1);
        QCOMPARE(lw[0].key, "sizeMode");
        QCOMPARE(lw[0].kind, Warning::Kind::WrongType);
        QCOMPARE(lw[0].effect, WarningEffect::StickerSkipped);
    }
    void invalidValueSizeMode() {
        QJsonObject obj{ { "id", "a" },{ "source", "a.png" },{ "sizeMode", "huge" } };
        QList<Warning> lw;
        auto st = parseSticker(obj, 0, lw);

        QCOMPARE(st.has_value(), false);
        QCOMPARE(lw.size(), 1);
        QCOMPARE(lw[0].key, "sizeMode");
        QCOMPARE(lw[0].kind, Warning::Kind::InvalidValue);
        QCOMPARE(lw[0].effect, WarningEffect::StickerSkipped);
    }
    void skipWrongScaleIfSizeModeSize() {
        QJsonObject obj{ { "id", "a" },{ "source", "a.png" },{ "scale", -3 } };
        QList<Warning> lw;
        auto st = parseSticker(obj, 0, lw);

        QVERIFY(st.has_value());
        QCOMPARE(lw.size(), 0);
        QCOMPARE(st->sizeMode, SizeMode::Size);
    }
    void outOfRangeWidthHeight() {
        QJsonObject obj{ { "id", "a" },{ "source", "a.png" },{ "width", -10 },{ "height", 0 } };
        QList<Warning> lw;
        auto st = parseSticker(obj, 0, lw);

        QCOMPARE(st.has_value(), false);
        QCOMPARE(lw.size(), 2);
        QCOMPARE(lw[0].key, "width");
        QCOMPARE(lw[0].kind, Warning::Kind::OutOfRange);
        QCOMPARE(lw[0].effect, WarningEffect::StickerSkipped);
        QCOMPARE(lw[1].key, "height");
        QCOMPARE(lw[1].kind, Warning::Kind::OutOfRange);
        QCOMPARE(lw[1].effect, WarningEffect::StickerSkipped);
    }
    void correctWidthHeight() {
        QJsonObject obj{ { "id", "a" },{ "source", "a.png" },{ "width", 300 },{ "height", 400 } };
        QList<Warning> lw;
        auto st = parseSticker(obj, 0, lw);

        QVERIFY(st.has_value());
        QCOMPARE(lw.size(), 0);
        QCOMPARE(st->width, 300);
        QCOMPARE(st->height, 400);
    }
    void outOfRangeScale() {
        QJsonObject obj{ { "id", "a" },{ "source", "a.png" },{ "sizeMode", "scale" },{ "scale", -10 } };
        QList<Warning> lw;
        auto st = parseSticker(obj, 0, lw);

        QCOMPARE(st.has_value(), false);
        QCOMPARE(lw.size(), 1);
        QCOMPARE(lw[0].key, "scale");
        QCOMPARE(lw[0].kind, Warning::Kind::OutOfRange);
        QCOMPARE(lw[0].effect, WarningEffect::StickerSkipped);
    }
    void wrongTypeSize() {
        QJsonObject obj{ { "id", "a" },{ "source", "a.png" },{ "width", "wide" } };
        QList<Warning> lw;
        auto st = parseSticker(obj, 0, lw);

        QCOMPARE(st.has_value(), false);
        QCOMPARE(lw.size(), 1);
        QCOMPARE(lw[0].key, "width");
        QCOMPARE(lw[0].kind, Warning::Kind::WrongType);
        QCOMPARE(lw[0].effect, WarningEffect::StickerSkipped);
    }
    void correctScaleAndDefaultSizes() {
        QJsonObject obj{ { "id", "a" },{ "source", "a.png" },{ "sizeMode", "scale" },{ "scale", 2 },
            { "width", 500 }, { "height", 150 } };
        QList<Warning> lw;
        auto st = parseSticker(obj, 0, lw);

        QVERIFY(st.has_value());
        QCOMPARE(lw.size(), 0);
        QCOMPARE(st->scale, 2);
        QCOMPARE(st->width, 200);
        QCOMPARE(st->height, 200);
    }
    void clampOpacity() {
        QJsonObject obj{ { "id", "a" },{ "source", "a.png" },{ "opacity", 20 } };
        QList<Warning> lw;
        auto st = parseSticker(obj, 0, lw);

        QVERIFY(st.has_value());
        QCOMPARE(st->opacity, 1);
        QCOMPARE(lw.size(), 1);
        QCOMPARE(lw[0].key, "opacity");
        QCOMPARE(lw[0].kind, Warning::Kind::Clamped);
        QCOMPARE(lw[0].effect, WarningEffect::ValueAdjusted);
    }
    void correctOpacity() {
        QJsonObject obj{ { "id", "a" },{ "source", "a.png" },{ "opacity", 0.5 } };
        QList<Warning> lw;
        auto st = parseSticker(obj, 0, lw);

        QVERIFY(st.has_value());
        QCOMPARE(lw.size(), 0);
        QCOMPARE(st->opacity, 0.5);
    }
    void wrongTypeOpacity() {
        QJsonObject obj{ { "id", "a" },{ "source", "a.png" },{ "opacity", "high" } };
        QList<Warning> lw;
        auto st = parseSticker(obj, 0, lw);

        QCOMPARE(st.has_value(), false);
        QCOMPARE(lw.size(), 1);
        QCOMPARE(lw[0].key, "opacity");
        QCOMPARE(lw[0].kind, Warning::Kind::WrongType);
        QCOMPARE(lw[0].effect, WarningEffect::StickerSkipped);
    }
    void correctRotation() {
        QJsonObject obj{ { "id", "a" },{ "source", "a.png" },{ "rotation", 45 } };
        QList<Warning> lw;
        auto st = parseSticker(obj, 0, lw);

        QVERIFY(st.has_value());
        QCOMPARE(lw.size(), 0);
        QCOMPARE(st->rotation, 45);
    }
    void unknownKeyStays() {
        QJsonObject obj{ { "id", "a" },{ "source", "a.png" },{ "depth", 20 } };
        QList<Warning> lw;
        auto st = parseSticker(obj, 0, lw);

        QVERIFY(st.has_value());
        QCOMPARE(lw.size(), 0);
        QCOMPARE(st->raw.contains("depth"), true);
    }
};

QTEST_APPLESS_MAIN(StickerTest)
#include "test_sticker.moc"