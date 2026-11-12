#include "stateLoader.hpp"
#include <QTemporaryDir>
#include <QFile>
#include <QIODevice>
#include <QFileDevice>
#include <QtTest>

using namespace mrst;

class LoadStateTest : public QObject {
    Q_OBJECT;

    static bool writeFile(const QString& _path, const QByteArray& _content) {
        QFile f(_path);
        return f.open(QIODevice::WriteOnly | QIODevice::Truncate)
            and f.write(_content) == _content.size();
    }

private slots:
    void validStateFile() {
        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        const QString path = dir.filePath("state.json");
        const QByteArray text = R"({ "schemaVersion": 1, "activeProfile": "x.json" })";
        QVERIFY(writeFile(path, text));

        auto res = loadStateFile(path);

        QVERIFY(res.state.has_value());
        QCOMPARE(res.warnings.size(), 0);
        QCOMPARE(res.state->schemaVersion, 1);
        QCOMPARE(res.state->activeProfile, "x.json");
    }
    void absentFile() {
        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        const QString path = dir.filePath("state_no_exist.json");

        auto res = loadStateFile(path);

        QCOMPARE(res.state.has_value(), false);
        QCOMPARE(res.warnings.size(), 0);
    }
    void emptyFile() {
        const QString path = QFINDTESTDATA("test_configs/empty_config.json");
        QVERIFY2(!path.isEmpty(), "fixture not found");
        
        auto res = loadStateFile(path);

        QCOMPARE(res.state.has_value(), false);
        QCOMPARE(res.warnings.size(), 1);
        QCOMPARE(res.warnings[0].where, "state");
        QCOMPARE(res.warnings[0].kind, Warning::Kind::InvalidJson);
        QCOMPARE(res.warnings[0].effect, WarningEffect::FileRejected);
    }
    void malformedJsonFile() {
        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        const QString path = dir.filePath("state.json");
        const QByteArray text = R"({ "schemaVersion": })";
        QVERIFY(writeFile(path, text));

        auto res = loadStateFile(path);

        QCOMPARE(res.state.has_value(), false);
        QCOMPARE(res.warnings.size(), 1);
        QCOMPARE(res.warnings[0].where, "state");
        QCOMPARE(res.warnings[0].kind, Warning::Kind::InvalidJson);
        QCOMPARE(res.warnings[0].effect, WarningEffect::FileRejected);
    }
    void existUnreadableFile() {
        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        const QString path = dir.filePath("state.json");

        const QByteArray text = R"({ "schemaVersion": 1, "activeProfile": "x.json" })";
        QVERIFY(writeFile(path, text));
        QFile f(path);
        QVERIFY(f.setPermissions(QFileDevice::Permissions{}));
        if (QFile(path).open(QIODevice::ReadOnly))
            QSKIP("permissions are not enforced here (running as root?)");

        auto res = loadStateFile(path);

        QCOMPARE(res.state.has_value(), false);
        QCOMPARE(res.warnings.size(), 1);
        QCOMPARE(res.warnings[0].where, "state");
        QCOMPARE(res.warnings[0].kind, Warning::Kind::Unreadable);
        QCOMPARE(res.warnings[0].effect, WarningEffect::FileRejected);
    }
};

QTEST_APPLESS_MAIN(LoadStateTest)
#include "test_load_state.moc"