#include "sticker.hpp"
#include <QtTest>

class StickerTest : public QObject {
    Q_OBJECT;

private slots:
    void happyPath() {
        QJsonObject obj{ { "id", "a" },{ "source", "a.png" },{ "x", 0 },{ "y", 0 } };
        QList<Warning> lw;
        auto st = parseSticker(obj, 0, lw);
        QString text;
        for (const auto warn : lw) {
            text += warn.where + warn.message;
        }
        QVERIFY2(st.has_value(), qPrintable(text));
        QCOMPARE(lw.size(), 0);
        QCOMPARE(st->id, "a");
        QCOMPARE(st->source, "a.png");
        QCOMPARE(st->x, 0);
        QCOMPARE(st->y, 0);
    }
    void missingRequiredField() {
        QJsonObject obj{ { "source", "a.png" },{ "x", 0 },{ "y", 0 } };
        QList<Warning> lw;
        auto st = parseSticker(obj, 0, lw);
        QString text;
        for (const auto warn : lw) {
            text += warn.where + warn.message;
        }
        QVERIFY2(st.has_value(), qPrintable(text));
        QCOMPARE(lw.size(), 1);
        QCOMPARE(lw[0].kind, Warning::Kind::Missing);
    }
    void wrongType() {
        QJsonObject obj{ { "id", 123 },{ "source", "a.png" },{ "x", "456" },{ "y", 0 } };
        QList<Warning> lw;
        auto st = parseSticker(obj, 0, lw);
        QString text;
        for (const auto warn : lw) {
            text += warn.where + warn.message;
        }
        QVERIFY2(st.has_value(), qPrintable(text));
        QCOMPARE(lw.size(), 2);
        QCOMPARE(lw[0].kind, Warning::Kind::WrongType);
        QCOMPARE(lw[1].kind, Warning::Kind::WrongType);
    }
    void nonEmptyProps() {
        QJsonObject obj{ { "id", "" },{ "source", "" },{ "x", 0 },{ "y", 0 } };
        QList<Warning> lw;
        auto st = parseSticker(obj, 0, lw);
        QString text;
        for (const auto warn : lw) {
            text += warn.where + warn.message;
        }
        QVERIFY2(st.has_value(), qPrintable(text));
        QCOMPARE(lw.size(), 2);
        QCOMPARE(lw[0].kind, Warning::Kind::Empty);
        QCOMPARE(lw[1].kind, Warning::Kind::Empty);
    }
    void optionalAndUniqueFields() {
        QJsonObject obj{ { "id", "a" },{ "source", "a.png" },{ "x", 0 },{ "y", 0 },
            { "enabled", false },{ "monitor", 5 } };
        QList<Warning> lw;
        auto st = parseSticker(obj, 0, lw);
        QString text;
        for (const auto warn : lw) {
            text += warn.where + warn.message;
        }
        QVERIFY2(st.has_value(), qPrintable(text));
        QCOMPARE(lw.size(), 0);
        QCOMPARE(st->id, "a");
        QCOMPARE(st->source, "a.png");
        QCOMPARE(st->enabled, false);
        QVERIFY(!st->monitor.has_value());
        QCOMPARE(st->x, 0);
        QCOMPARE(st->y, 0);
        QCOMPARE(st->sizeMode, SizeMode::Size);
        QCOMPARE(st->width, 200);
        QCOMPARE(st->height, 200);
        QCOMPARE(st->scale, 1.0);
        QCOMPARE(st->rotation, 0);
        QCOMPARE(st->opacity, 1);
    }
};

QTEST_APPLESS_MAIN(StickerTest)
#include "test_sticker.moc"