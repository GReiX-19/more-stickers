#include "sticker.hpp"
#include <QtTest>

class StickerTest : public QObject {
    Q_OBJECT;

private slots:
    void opacityIsClampedWithWarning() {
        QJsonObject o{ { "id", "a" },{ "source", "a.png" },{ "x", 0 },{ "y", 0 },{ "opacity", 10 } };
        QList<Warning> w;
        auto s = parseSticker(o, 0, w);
        QVERIFY(s.has_value());
        QCOMPARE(s->opacity, 1.0);
        QCOMPARE(w.size(), 1);
    }
};

QTEST_APPLESS_MAIN(StickerTest)
#include "test_sticker.moc"