#include "json_read.hpp"
#include "json_warn.hpp"
#include "Warning.hpp"
#include <QtTest>

using namespace mrst;

class ReadJsonTest : public QObject {
    Q_OBJECT;

private slots:
    void correctReadInt() {
        QJsonObject obj{ { "schemaVersion", 1 } };
        QList<Warning> lw;
        ParseContext ctx{ "profile", WarningEffect::FileRejected, lw };

        int out = 0;
        bool ok = readInt(obj, "schemaVersion", FieldOption::Required, out, ctx);

        QVERIFY(ok);
        QCOMPARE(out, 1);
        QCOMPARE(lw.size(), 0);
    }
    void stringReadInt() {
        QJsonObject obj{ { "schemaVersion", "1" } };
        QList<Warning> lw;
        ParseContext ctx{ "profile", WarningEffect::FileRejected, lw };

        int out = 0;
        bool ok = readInt(obj, "schemaVersion", FieldOption::Required, out, ctx);

        QVERIFY(!ok);
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

        QVERIFY(!ok);
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

        QVERIFY(!ok);
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

        QVERIFY(ok);
        QCOMPARE(out, 0);
        QCOMPARE(lw.size(), 0);
    }
    void exponentialReadInt() {
        QJsonObject obj{ { "schemaVersion", 1e20 } };
        QList<Warning> lw;
        ParseContext ctx{ "profile", WarningEffect::FileRejected, lw };

        int out = 0;
        bool ok = readInt(obj, "schemaVersion", FieldOption::Required, out, ctx);

        QVERIFY(!ok);
        QCOMPARE(out, 0);
        QCOMPARE(lw.size(), 1);
        QCOMPARE(lw[0].key, "schemaVersion");
        QCOMPARE(lw[0].kind, Warning::Kind::WrongType);
        QCOMPARE(lw[0].effect, WarningEffect::FileRejected);
    }
        void wrongTypeMessageString() {
        QJsonObject obj{ { "schemaVersion", "1" } };
        QList<Warning> lw;
        ParseContext ctx{ "profile", WarningEffect::FileRejected, lw };

        int out = 0;
        readInt(obj, "schemaVersion", FieldOption::Required, out, ctx);

        QCOMPARE(lw.size(), 1);
        QCOMPARE(lw[0].message, "expected integer, found string \"1\"");
    }
    void wrongTypeMessageNumber() {
        QJsonObject obj{ { "schemaVersion", 1.5 } };
        QList<Warning> lw;
        ParseContext ctx{ "profile", WarningEffect::FileRejected, lw };

        int out = 0;
        readInt(obj, "schemaVersion", FieldOption::Required, out, ctx);

        QCOMPARE(lw.size(), 1);
        QCOMPARE(lw[0].message, "expected integer, found number \"1.5\"");
    }
    void wrongTypeMessageBool() {
        QJsonObject obj{ { "schemaVersion", true } };
        QList<Warning> lw;
        ParseContext ctx{ "profile", WarningEffect::FileRejected, lw };

        int out = 0;
        readInt(obj, "schemaVersion", FieldOption::Required, out, ctx);

        QCOMPARE(lw.size(), 1);
        QCOMPARE(lw[0].message, "expected integer, found bool \"true\"");
    }
    void wrongTypeMessageNull() {
        QJsonObject obj{ { "schemaVersion", QJsonValue::Null } };
        QList<Warning> lw;
        ParseContext ctx{ "profile", WarningEffect::FileRejected, lw };

        int out = 0;
        readInt(obj, "schemaVersion", FieldOption::Required, out, ctx);

        QCOMPARE(lw.size(), 1);
        QCOMPARE(lw[0].message, "expected integer, found null");
    }
    void wrongTypeMessageArray() {
        QJsonObject obj{ { "schemaVersion", QJsonArray{ 1 } } };
        QList<Warning> lw;
        ParseContext ctx{ "profile", WarningEffect::FileRejected, lw };

        int out = 0;
        readInt(obj, "schemaVersion", FieldOption::Required, out, ctx);

        QCOMPARE(lw.size(), 1);
        QCOMPARE(lw[0].message, "expected integer, found array");
    }
    void wrongTypeMessageObject() {
        QJsonObject obj{ { "schemaVersion", QJsonObject{ { "a", 1 } } } };
        QList<Warning> lw;
        ParseContext ctx{ "profile", WarningEffect::FileRejected, lw };

        int out = 0;
        readInt(obj, "schemaVersion", FieldOption::Required, out, ctx);

        QCOMPARE(lw.size(), 1);
        QCOMPARE(lw[0].message, "expected integer, found object");
    }
    void wrongTypeMessageUndefined() {
        QList<Warning> lw;
        ParseContext ctx{ "profile", WarningEffect::FileRejected, lw };

        warnWrongType("", "object", QJsonValue{ QJsonValue::Undefined }, ctx);

        QCOMPARE(lw.size(), 1);
        QCOMPARE(lw[0].message, "expected object, found undefined");
    }
    void wrongTypeMessageDefaultValueIsNull() {
        QList<Warning> lw;
        ParseContext ctx{ "profile", WarningEffect::FileRejected, lw };

        warnWrongType("", "object", QJsonValue{}, ctx);

        QCOMPARE(lw.size(), 1);
        QCOMPARE(lw[0].message, "expected object, found null");
    }
    void invalidFileNameMessage() {
        QList<Warning> lw;
        ParseContext ctx{ "state", WarningEffect::FileRejected, lw };

        warnInvalidFileName("activeProfile", ctx);

        QCOMPARE(lw.size(), 1);
        QCOMPARE(lw[0].where, "state");
        QCOMPARE(lw[0].key, "activeProfile");
        QCOMPARE(lw[0].kind, Warning::Kind::InvalidValue);
        QCOMPARE(lw[0].effect, WarningEffect::FileRejected);
        QCOMPARE(lw[0].message, "must be a plain file name (no '/', not '.' or '..')");
    }
};

QTEST_APPLESS_MAIN(ReadJsonTest)
#include "test_json_read.moc"