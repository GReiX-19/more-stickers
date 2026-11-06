#include "json_read.hpp"
#include "Warning.hpp"
#include <QtTest>

using namespace mrst;

class JsonReadTest : public QObject {
    Q_OBJECT;

private slots:
    void correctReadInt() {
        QJsonObject obj{ { "schemaVersion", 1 } };
        QList<Warning> lw;
        ParseContext ctx{ "profile", WarningEffect::FileRejected, lw };

        int out = 0;
        bool ok = readInt(obj, "schemaVersion", FieldOption::Required, out, ctx);

        QCOMPARE(ok, true);
        QCOMPARE(out, 1);
        QCOMPARE(lw.size(), 0);
    }
    void stringReadInt() {
        QJsonObject obj{ { "schemaVersion", "1" } };
        QList<Warning> lw;
        ParseContext ctx{ "profile", WarningEffect::FileRejected, lw };

        int out = 0;
        bool ok = readInt(obj, "schemaVersion", FieldOption::Required, out, ctx);

        QCOMPARE(ok, false);
        QCOMPARE(out, 0);
        QCOMPARE(lw.size(), 1);
        QCOMPARE(lw[0].key, "schemaVersion");
        QCOMPARE(lw[0].kind, Warning::Kind::WrongType);
        QCOMPARE(lw[0].effect, WarningEffect::FileRejected);
    }
    void doubleReadInt() {
        QJsonObject obj{ { "schemaVersion", 1.5 } };
        QList<Warning> lw;
        ParseContext ctx{ "profile", WarningEffect::FileRejected, lw };

        int out = 0;
        bool ok = readInt(obj, "schemaVersion", FieldOption::Required, out, ctx);

        QCOMPARE(ok, false);
        QCOMPARE(out, 0);
        QCOMPARE(lw.size(), 1);
        QCOMPARE(lw[0].key, "schemaVersion");
        QCOMPARE(lw[0].kind, Warning::Kind::WrongType);
        QCOMPARE(lw[0].effect, WarningEffect::FileRejected);
    }
    void requiredFieldReadInt() {
        QJsonObject obj{};
        QList<Warning> lw;
        ParseContext ctx{ "profile", WarningEffect::FileRejected, lw };

        int out = 0;
        bool ok = readInt(obj, "schemaVersion", FieldOption::Required, out, ctx);

        QCOMPARE(ok, false);
        QCOMPARE(out, 0);
        QCOMPARE(lw.size(), 1);
        QCOMPARE(lw[0].key, "schemaVersion");
        QCOMPARE(lw[0].kind, Warning::Kind::Missing);
        QCOMPARE(lw[0].effect, WarningEffect::FileRejected);
    }
    void notRequiredFieldReadInt() {
        QJsonObject obj{};
        QList<Warning> lw;
        ParseContext ctx{ "profile", WarningEffect::FileRejected, lw };

        int out = 0;
        bool ok = readInt(obj, "schemaVersion", FieldOptions{}, out, ctx);

        QCOMPARE(ok, true);
        QCOMPARE(out, 0);
        QCOMPARE(lw.size(), 0);
    }
    void exponentialReadInt() {
        QJsonObject obj{ { "schemaVersion", 1e20 } };
        QList<Warning> lw;
        ParseContext ctx{ "profile", WarningEffect::FileRejected, lw };

        int out = 0;
        bool ok = readInt(obj, "schemaVersion", FieldOption::Required, out, ctx);

        QCOMPARE(ok, false);
        QCOMPARE(out, 0);
        QCOMPARE(lw.size(), 1);
        QCOMPARE(lw[0].key, "schemaVersion");
        QCOMPARE(lw[0].kind, Warning::Kind::WrongType);
        QCOMPARE(lw[0].effect, WarningEffect::FileRejected);
    }
};

QTEST_APPLESS_MAIN(JsonReadTest)
#include "test_json_read.moc"