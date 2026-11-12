#include "state.hpp"
#include "Warning.hpp"
#include <QtTest>

using namespace mrst;

class ParseStateTest : public QObject {
    Q_OBJECT;

private slots:
    void validState() {
        QJsonObject obj{ { "schemaVersion", 1 },{ "activeProfile", "profile.json" } };
        QList<Warning> lw;
        auto state = parseState(obj, lw);

        QVERIFY(state.has_value());
        QCOMPARE(lw.size(), 0);
        QCOMPARE(state->schemaVersion, 1);
        QCOMPARE(state->activeProfile, "profile.json");
    }
    void badSchemaVersion() {
        QJsonObject obj{ { "schemaVersion", 2 } };
        QList<Warning> lw;
        auto state = parseState(obj, lw);

        QCOMPARE(state.has_value(), false);
        QCOMPARE(lw.size(), 1);
        QCOMPARE(lw[0].key, "schemaVersion");
        QCOMPARE(lw[0].kind, Warning::Kind::InvalidValue);
        QCOMPARE(lw[0].effect, WarningEffect::FileRejected);
    }
    void missingSchemaVersion() {
        QJsonObject obj{};
        QList<Warning> lw;
        auto state = parseState(obj, lw);

        QCOMPARE(state.has_value(), false);
        QCOMPARE(lw.size(), 1);
        QCOMPARE(lw[0].key, "schemaVersion");
        QCOMPARE(lw[0].kind, Warning::Kind::Missing);
        QCOMPARE(lw[0].effect, WarningEffect::FileRejected);
    }
    void oneWarning() {
        QJsonObject obj{ { "schemaVersion", "1" },{ "activeProfile", 42 } };
        QList<Warning> lw;
        auto state = parseState(obj, lw);

        QCOMPARE(state.has_value(), false);
        QCOMPARE(lw.size(), 1);
        QCOMPARE(lw[0].key, "schemaVersion");
        QCOMPARE(lw[0].kind, Warning::Kind::WrongType);
        QCOMPARE(lw[0].effect, WarningEffect::FileRejected);
    }
    void missingActiveProfile() {
        QJsonObject obj{ { "schemaVersion", 1 } };
        QList<Warning> lw;
        auto state = parseState(obj, lw);

        QCOMPARE(state.has_value(), false);
        QCOMPARE(lw.size(), 1);
        QCOMPARE(lw[0].key, "activeProfile");
        QCOMPARE(lw[0].kind, Warning::Kind::Missing);
        QCOMPARE(lw[0].effect, WarningEffect::FileRejected);
    }
    void wrongTypeActiveProfile() {
        QJsonObject obj{ { "schemaVersion", 1 },{ "activeProfile", 42 } };
        QList<Warning> lw;
        auto state = parseState(obj, lw);

        QCOMPARE(state.has_value(), false);
        QCOMPARE(lw.size(), 1);
        QCOMPARE(lw[0].key, "activeProfile");
        QCOMPARE(lw[0].kind, Warning::Kind::WrongType);
        QCOMPARE(lw[0].effect, WarningEffect::FileRejected);
    }
    void emptyActiveProfile() {
        QJsonObject obj{ { "schemaVersion", 1 },{ "activeProfile", "" } };
        QList<Warning> lw;
        auto state = parseState(obj, lw);

        QCOMPARE(state.has_value(), false);
        QCOMPARE(lw.size(), 1);
        QCOMPARE(lw[0].key, "activeProfile");
        QCOMPARE(lw[0].kind, Warning::Kind::Empty);
        QCOMPARE(lw[0].effect, WarningEffect::FileRejected);
    }
    void invalidActiveProfileNames_data() {
        QTest::addColumn<QString>("name");

        QTest::newRow("parent traversal") << "../x.json";
        QTest::newRow("subfolder") << "sub/x.json";
        QTest::newRow("absolute path") << "/x.json";
        QTest::newRow("single dot") << ".";
        QTest::newRow("double dot") << "..";
    }
    void invalidActiveProfileNames() {
        QFETCH(QString, name);

        QJsonObject obj{ { "schemaVersion", 1 },{ "activeProfile", name } };
        QList<Warning> lw;
        auto state = parseState(obj, lw);

        QCOMPARE(state.has_value(), false);
        QCOMPARE(lw.size(), 1);
        QCOMPARE(lw[0].key, "activeProfile");
        QCOMPARE(lw[0].kind, Warning::Kind::InvalidValue);
        QCOMPARE(lw[0].effect, WarningEffect::FileRejected);
    }
    void validActiveProfileNames_data() {
        QTest::addColumn<QString>("name");

        QTest::newRow("regular") << "work.json";
        QTest::newRow("hidden") << ".hidden.json";
    }
    void validActiveProfileNames() {
        QFETCH(QString, name);

        QJsonObject obj{ { "schemaVersion", 1 },{ "activeProfile", name } };
        QList<Warning> lw;
        auto state = parseState(obj, lw);

        QVERIFY(state.has_value());
        QCOMPARE(lw.size(), 0);
        QCOMPARE(state->activeProfile, name);
    }
};

QTEST_APPLESS_MAIN(ParseStateTest)
#include "test_parse_state.moc"