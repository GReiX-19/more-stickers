#include "sticker.hpp"
#include <QtTest>

class StickerTest : public QObject {
    Q_OBJECT;

private slots:
    void happyPath() {
        QJsonObject obj{ { "id", "a" },{ "source", "a.png" },{ "x", 0 },{ "y", 0 } };
        QList<Warning> lw;
        auto st = parseSticker(obj, 0, lw);
        QCOMPARE(st.has_value(), true);
        QCOMPARE(lw.size(), 0);
    }
    void missingRequiredField() {
        QJsonObject obj{ { "source", "a.png" },{ "x", 0 },{ "y", 0 } };
        QList<Warning> lw;
        auto st = parseSticker(obj, 0, lw);
        QCOMPARE(st.has_value(), false);
        QCOMPARE(lw.size(), 1);
    }
    void wrongType() {
        QJsonObject obj{ { "id", 123 },{ "source", "a.png" },{ "x", "456" },{ "y", 0 } };
        QList<Warning> lw;
        auto st = parseSticker(obj, 0, lw);
        QCOMPARE(st.has_value(), false);
        QCOMPARE(lw.size(), 2);
    }
};

QTEST_APPLESS_MAIN(StickerTest)
#include "test_sticker.moc"