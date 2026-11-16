#include "stateActions.hpp"
#include "stateLoader.hpp"
#include "appPaths.hpp"
#include "helpers.hpp"
#include <QTemporaryDir>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QIODevice>
#include <QScopeGuard>
#include <QTest>

using namespace mrst;

class StateActionsTest : public QObject {
    Q_OBJECT;

private slots:
    void validNameExistingProfile() {
        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        const auto paths = pathsUnder(dir.path(), dir.path());
        QVERIFY(QDir().mkpath(paths.profilesDir()));
        QVERIFY(writeFile(paths.profilesDir() + "/work.json", "{}"));

        QVERIFY(!setActiveProfile(paths, "work.json").has_value());

        auto res = loadStateFile(paths.stateFile());
        QVERIFY(res.state.has_value());
        QCOMPARE(res.warnings.size(), 0);
        QCOMPARE(res.state->activeProfile, "work.json");
    }
    void absentProfile() {
        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        const auto paths = pathsUnder(dir.path(), dir.path());
        QVERIFY(QDir().mkpath(paths.profilesDir()));
        const QByteArray text = R"({ "schemaVersion": 1, "activeProfile": "x.json" })";
        QVERIFY(writeFile(paths.stateFile(), text));

        auto err = setActiveProfile(paths, "no_exist.json");
        QVERIFY(err.has_value());
        QVERIFY(err->contains("does not exist"));
        QCOMPARE(readFile(paths.stateFile()), text);
    }
    void unsafeProfileName() {
        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        const auto paths = pathsUnder(dir.path(), dir.path());
        QVERIFY(QDir().mkpath(paths.profilesDir()));
        const QByteArray text = R"({ "schemaVersion": 1, "activeProfile": "x.json" })";
        QVERIFY(writeFile(paths.stateFile(), text));
        QVERIFY(writeFile(paths.configDir + "/unsafe.json", "{}"));

        auto err = setActiveProfile(paths, "../unsafe.json");
        QVERIFY(err.has_value());
        QVERIFY(err->contains("must be non-empty plain file name"));
        QCOMPARE(readFile(paths.stateFile()), text);
    }
    void directoryProfile() {
        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        const auto paths = pathsUnder(dir.path(), dir.path());
        QVERIFY(QDir().mkpath(paths.profilesDir() + "/directoryTo"));
        const QByteArray text = R"({ "schemaVersion": 1, "activeProfile": "x.json" })";
        QVERIFY(writeFile(paths.stateFile(), text));

        auto err = setActiveProfile(paths, "directoryTo");
        QVERIFY(err.has_value());
        QVERIFY(err->contains("must be a file"));
        QCOMPARE(readFile(paths.stateFile()), text);
    }
    void saveFailure() {
        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        const auto paths = pathsUnder(dir.path(), dir.path());
        QVERIFY(QDir().mkpath(paths.profilesDir()));
        QVERIFY(writeFile(paths.profilesDir() + "/work.json", "{}"));
        const QByteArray text = R"({ "schemaVersion": 1, "activeProfile": "x.json" })";
        QVERIFY(writeFile(paths.stateFile(), text));

        ReadOnlyDir lock(paths.configDir);
        if (!lock.enforced())
            QSKIP("permissions are not enforced here (running as root?)");
        QVERIFY(QFileInfo(paths.profilesDir() + "/work.json").isFile());

        auto err = setActiveProfile(paths, "directoryTo");
        QVERIFY(err.has_value());
        QVERIFY(!err->isEmpty());
        QCOMPARE(readFile(paths.stateFile()), text);
    }
    void overwriteExistingState() {
        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        const auto paths = pathsUnder(dir.path(), dir.path());
        QVERIFY(QDir().mkpath(paths.profilesDir()));
        QVERIFY(writeFile(paths.profilesDir() + "/x.json", "{}"));
        QVERIFY(writeFile(paths.profilesDir() + "/y.json", "{}"));
        const QByteArray text = R"({ "schemaVersion": 1, "activeProfile": "x.json" })";
        QVERIFY(writeFile(paths.stateFile(), text));

        QVERIFY(!setActiveProfile(paths, "y.json").has_value());

        auto res = loadStateFile(paths.stateFile());
        QVERIFY(res.state.has_value());
        QCOMPARE(res.warnings.size(), 0);
        QCOMPARE(res.state->activeProfile, "y.json");
    }
    void emptyName() {
        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        const auto paths = pathsUnder(dir.path(), dir.path());
        QVERIFY(QDir().mkpath(paths.profilesDir()));
        const QByteArray text = R"({ "schemaVersion": 1, "activeProfile": "x.json" })";
        QVERIFY(writeFile(paths.stateFile(), text));

        auto err = setActiveProfile(paths, "");
        QVERIFY(err.has_value());
        QVERIFY(err->contains("must be non-empty plain file name"));
        QCOMPARE(readFile(paths.stateFile()), text);
    }
};

QTEST_APPLESS_MAIN(StateActionsTest)
#include "test_state_actions.moc"