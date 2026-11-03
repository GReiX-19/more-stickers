#include "sticker.hpp"
#include <QtTest>

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
        QCOMPARE(lw[0].kind, Warning::Kind::Missing);
    }
    void wrongType() {
        QJsonObject obj{ { "id", 123 },{ "source", "a.png" },{ "x", "456" } };
        QList<Warning> lw;
        auto st = parseSticker(obj, 0, lw);

        QCOMPARE(st.has_value(), false);
        QCOMPARE(lw.size(), 2);
        QCOMPARE(lw[0].kind, Warning::Kind::WrongType);
        QCOMPARE(lw[1].kind, Warning::Kind::WrongType);
    }
    void nonEmptyProps() {
        QJsonObject obj{ { "id", "" },{ "source", "" } };
        QList<Warning> lw;
        auto st = parseSticker(obj, 0, lw);

        QCOMPARE(st.has_value(), false);
        QCOMPARE(lw.size(), 2);
        QCOMPARE(lw[0].kind, Warning::Kind::Empty);
        QCOMPARE(lw[1].kind, Warning::Kind::Empty);
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
    void emptyMonitorName() {
        QJsonObject obj{ { "id", "a" },{ "source", "a.png" },{ "monitor", "" } };
        QList<Warning> lw;
        auto st = parseSticker(obj, 0, lw);

        QVERIFY(st.has_value());
        QCOMPARE(lw.size(), 0);
        QCOMPARE(st->width, 200);
        QCOMPARE(st->monitor, std::nullopt);
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
        QCOMPARE(lw[0].kind, Warning::Kind::WrongType);
    }
    void invalidValueSizeMode() {
        QJsonObject obj{ { "id", "a" },{ "source", "a.png" },{ "sizeMode", "huge" } };
        QList<Warning> lw;
        auto st = parseSticker(obj, 0, lw);

        QCOMPARE(st.has_value(), false);
        QCOMPARE(lw.size(), 1);
        QCOMPARE(lw[0].kind, Warning::Kind::InvalidValue);
    }
    void outOfRangeWidthHeight() {
        QJsonObject obj{ { "id", "a" },{ "source", "a.png" },{ "width", -10 },{ "height", - 10 } };
        QList<Warning> lw;
        auto st = parseSticker(obj, 0, lw);

        QCOMPARE(st.has_value(), false);
        QCOMPARE(lw.size(), 2);
        QCOMPARE(lw[0].kind, Warning::Kind::OutOfRange);
        QCOMPARE(lw[1].kind, Warning::Kind::OutOfRange);
    }
    void outOfRangeScale() {
        QJsonObject obj{ { "id", "a" },{ "source", "a.png" },{ "sizeMode", "scale" },{ "scale", -10 } };
        QList<Warning> lw;
        auto st = parseSticker(obj, 0, lw);

        QCOMPARE(st.has_value(), false);
        QCOMPARE(lw.size(), 1);
        QCOMPARE(lw[0].kind, Warning::Kind::OutOfRange);
    }
    void clampOpacity() {
        QJsonObject obj{ { "id", "a" },{ "source", "a.png" },{ "opacity", 20 } };
        QList<Warning> lw;
        auto st = parseSticker(obj, 0, lw);

        QCOMPARE(st.has_value(), true);
        QCOMPARE(st->opacity, 1);
        QCOMPARE(lw.size(), 1);
        QCOMPARE(lw[0].kind, Warning::Kind::Clamped);
    }
    void unknownKeyStays() {
        QJsonObject obj{ { "id", "a" },{ "source", "a.png" },{ "depth", 20 } };
        QList<Warning> lw;
        auto st = parseSticker(obj, 0, lw);

        QCOMPARE(st.has_value(), true);
        QCOMPARE(lw.size(), 0);
        QCOMPARE(st->raw.contains("depth"), true);
    }
};

QTEST_APPLESS_MAIN(StickerTest)
#include "test_sticker.moc"