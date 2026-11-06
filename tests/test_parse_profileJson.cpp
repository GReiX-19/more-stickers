#include "profileJson.hpp"
#include "Warning.hpp"
#include <QtTest>

using namespace mrst;

class ParseProfileJson : public QObject {
    Q_OBJECT;

private slots:
    void validProfile() {
        QJsonObject obj{ { "schemaVersion", 1 },{ "name", "prof1" },
            { "stickers", QJsonArray{ QJsonObject{ { "id", "a" },{ "source", "a.png" } } } } };
        QList<Warning> lw;
        auto p = parseProfile(obj, lw);

        QVERIFY(p.has_value());
        QCOMPARE(lw.size(), 0);
        QCOMPARE(p->schemaVersion, 1);
        QCOMPARE(p->name, "prof1");
        QCOMPARE(p->stickers.size(), 1);
    }
    void stringVersion() {
        QJsonObject obj{ { "schemaVersion", "1" } };
        QList<Warning> lw;
        auto p = parseProfile(obj, lw);

        QCOMPARE(p.has_value(), false);
        QCOMPARE(lw.size(), 1);
        QCOMPARE(lw[0].key, "schemaVersion");
        QCOMPARE(lw[0].kind, Warning::Kind::WrongType);
        QCOMPARE(lw[0].effect, WarningEffect::FileRejected);
    }
    void incorrectVersion() {
        QJsonObject obj{ { "schemaVersion", 2 } };
        QList<Warning> lw;
        auto p = parseProfile(obj, lw);

        QCOMPARE(p.has_value(), false);
        QCOMPARE(lw.size(), 1);
        QCOMPARE(lw[0].key, "schemaVersion");
        QCOMPARE(lw[0].kind, Warning::Kind::InvalidValue);
        QCOMPARE(lw[0].effect, WarningEffect::FileRejected);
    }
    void doubleVersion() {
        QJsonObject obj{ { "schemaVersion", 2.5 } };
        QList<Warning> lw;
        auto p = parseProfile(obj, lw);

        QCOMPARE(p.has_value(), false);
        QCOMPARE(lw.size(), 1);
        QCOMPARE(lw[0].key, "schemaVersion");
        QCOMPARE(lw[0].kind, Warning::Kind::WrongType);
        QCOMPARE(lw[0].effect, WarningEffect::FileRejected);
    }
    void wrongTypeName() {
        QJsonObject obj{ { "schemaVersion", 1 },{ "name", 1 } };
        QList<Warning> lw;
        auto p = parseProfile(obj, lw);

        QCOMPARE(p.has_value(), false);
        QCOMPARE(lw.size(), 1);
        QCOMPARE(lw[0].key, "name");
        QCOMPARE(lw[0].kind, Warning::Kind::WrongType);
        QCOMPARE(lw[0].effect, WarningEffect::FileRejected);
    }
    void missingSticker() {
        QJsonObject obj{ { "schemaVersion", 1 } };
        QList<Warning> lw;
        auto p = parseProfile(obj, lw);

        QCOMPARE(p.has_value(), false);
        QCOMPARE(lw.size(), 1);
        QCOMPARE(lw[0].key, "stickers");
        QCOMPARE(lw[0].kind, Warning::Kind::Missing);
        QCOMPARE(lw[0].effect, WarningEffect::FileRejected);
    }
    void wrongTypeStickers() {
        QJsonObject obj{ { "schemaVersion", 1 },{ "stickers", "x" } };
        QList<Warning> lw;
        auto p = parseProfile(obj, lw);

        QCOMPARE(p.has_value(), false);
        QCOMPARE(lw.size(), 1);
        QCOMPARE(lw[0].key, "stickers");
        QCOMPARE(lw[0].kind, Warning::Kind::WrongType);
        QCOMPARE(lw[0].effect, WarningEffect::FileRejected);
    }
    void nonObjectStickers() {
        QJsonObject obj{ { "schemaVersion", 1 },{ "stickers", QJsonArray{ 5,
            QJsonObject{ { "id", "a" },{ "source", "a.png" } } } } };
        QList<Warning> lw;
        auto p = parseProfile(obj, lw);

        QVERIFY(p.has_value());
        QCOMPARE(lw.size(), 1);
        QCOMPARE(lw[0].where, "stickers[0]");
        QCOMPARE(lw[0].key, "");
        QCOMPARE(lw[0].kind, Warning::Kind::WrongType);
        QCOMPARE(lw[0].effect, WarningEffect::StickerSkipped);
    }
    void emptyStickers() {
        QJsonObject obj{ { "schemaVersion", 1 },{ "stickers", QJsonArray{} } };
        QList<Warning> lw;
        auto p = parseProfile(obj, lw);

        QVERIFY(p.has_value());
        QCOMPARE(lw.size(), 0);
    }
    void duplicateId() {
        QJsonObject obj{ { "schemaVersion", 1 },
            { "stickers", QJsonArray{
                QJsonObject{ { "id", "a" },{ "source", "a.png" } },
                QJsonObject{ { "id", "b" },{ "source", "b.png" } },
                QJsonObject{ { "id", "a" },{ "source", "a.png" } },
                }
            }
        };
        QList<Warning> lw;
        auto p = parseProfile(obj, lw);

        QVERIFY(p.has_value());
        QCOMPARE(p->stickers.size(), 2);
        QCOMPARE(lw.size(), 1);
        QCOMPARE(lw[0].where, "stickers[2]");
        QCOMPARE(lw[0].key, "id");
        QCOMPARE(lw[0].kind, Warning::Kind::Duplicate);
        QCOMPARE(lw[0].effect, WarningEffect::StickerSkipped);
    }
    void invalidValidSticker() {
        QJsonObject obj{ { "schemaVersion", 1 },
            { "stickers", QJsonArray{
                QJsonObject{ { "id", "a" },{ "source", 1 } },
                QJsonObject{ { "id", "a" },{ "source", "a.png" } },
                }
            }
        };
        QList<Warning> lw;
        auto p = parseProfile(obj, lw);

        QVERIFY(p.has_value());
        QCOMPARE(p->stickers.size(), 1);
        QCOMPARE(lw.size(), 1);
        QCOMPARE(lw[0].where, "stickers[0] (id: \"a\")");
        QCOMPARE(lw[0].key, "source");
        QCOMPARE(lw[0].kind, Warning::Kind::WrongType);
        QCOMPARE(lw[0].effect, WarningEffect::StickerSkipped);
    }
    void unknownKey() {
        QJsonObject obj{ { "schemaVersion", 1 },{ "path", "some path" },
            { "stickers", QJsonArray{ QJsonObject{ { "id", "a" },{ "source", "a.png" } } } } };
        QList<Warning> lw;
        auto p = parseProfile(obj, lw);

        QVERIFY(p.has_value());
        QCOMPARE(lw.size(), 0);
        QCOMPARE(p->raw, obj);
    }
};

QTEST_APPLESS_MAIN(ParseProfileJson)
#include "test_parse_profileJson.moc"