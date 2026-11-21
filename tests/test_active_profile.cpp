#include "activeProfile.hpp"
#include "appPaths.hpp"
#include "helpers.hpp"
#include "Warning.hpp"
#include <QTemporaryDir>
#include <QDir>
#include <QTest>

using namespace mrst;
using namespace mrst::tests;

class ActiveProfileTest : public QObject {
    Q_OBJECT;

private slots:
    void noStateFile() {
        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        const auto paths = pathsUnder(dir.path(), dir.path());
        QVERIFY(QDir().mkpath(paths.profilesDir()));

        auto res = resolveActiveProfile(paths);
        QVERIFY(!res.path.has_value());
        QCOMPARE(res.warnings.size(), 0);
        QCOMPARE(res.status, ActiveProfile::Status::NoState);
    }
    void validState() {
        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        const auto paths = pathsUnder(dir.path(), dir.path());
        QVERIFY(QDir().mkpath(paths.profilesDir()));
        const QByteArray text = R"({ "schemaVersion": 1, "activeProfile": "x.json" })";
        QVERIFY(writeFile(paths.stateFile(), text));

        auto res = resolveActiveProfile(paths);
        QVERIFY(res.path.has_value());
        QCOMPARE(res.warnings.size(), 0);
        QCOMPARE(res.status, ActiveProfile::Status::Ready);
        QCOMPARE(res.path.value(), paths.profilePath("x.json"));
    }
    void malformedState() {
        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        const auto paths = pathsUnder(dir.path(), dir.path());
        QVERIFY(QDir().mkpath(paths.configDir));
        QVERIFY(writeFile(paths.stateFile(), R"({ "schemaVersion": })"));

        auto res = resolveActiveProfile(paths);
        QVERIFY(!res.path.has_value());
        QCOMPARE(res.warnings.size(), 1);
        QCOMPARE(res.warnings[0].kind, Warning::Kind::InvalidJson);
        QCOMPARE(res.status, ActiveProfile::Status::Rejected);
    }
    void unsafeActiveProfile() {
        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        const auto paths = pathsUnder(dir.path(), dir.path());
        QVERIFY(QDir().mkpath(paths.configDir));
        const QByteArray text = R"({ "schemaVersion": 1, "activeProfile": "../x.json" })";
        QVERIFY(writeFile(paths.stateFile(), text));

        auto res = resolveActiveProfile(paths);
        QVERIFY(!res.path.has_value());
        QCOMPARE(res.warnings.size(), 1);
        QCOMPARE(res.warnings[0].where, "state");
        QCOMPARE(res.warnings[0].key, "activeProfile");
        QCOMPARE(res.warnings[0].kind, Warning::Kind::InvalidValue);
        QCOMPARE(res.warnings[0].effect, WarningEffect::FileRejected);
        QCOMPARE(res.status, ActiveProfile::Status::Rejected);
    }
    void rejectsFileEmptyList() {
        QVERIFY(!rejectsFile(QList<Warning>{}));
    }
    void rejectsFileIgnoresNonFatalWarnings() {
        const QList<Warning> warnings{
            { .effect = WarningEffect::ValueAdjusted },
            { .effect = WarningEffect::StickerSkipped }
        };
        QVERIFY(!rejectsFile(warnings));
    }
    void rejectsFileDetectsFileRejected() {
        const QList<Warning> warnings{ { .effect = WarningEffect::FileRejected } };
        QVERIFY(rejectsFile(warnings));
    }
    void rejectsFileFindsItAmongOthers() {
        const QList<Warning> warnings{
            { .effect = WarningEffect::ValueAdjusted },
            { .effect = WarningEffect::FileRejected },
            { .effect = WarningEffect::StickerSkipped }
        };
        QVERIFY(rejectsFile(warnings));
    }
};

QTEST_APPLESS_MAIN(ActiveProfileTest)
#include "test_active_profile.moc"