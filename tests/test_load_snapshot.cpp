#include "snapshotLoader.hpp"
#include "activeProfile.hpp"
#include "appPaths.hpp"
#include "helpers.hpp"
#include <QTemporaryDir>
#include <QDir>
#include <QTest>

using namespace mrst;
using namespace mrst::tests;

class LoadSnapshotTest : public QObject {
    Q_OBJECT;

private slots:
    void noStateFile() {
        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        const auto paths = pathsUnder(dir.path(), dir.path());
        QVERIFY(QDir().mkpath(paths.profilesDir()));

        auto res = loadSnapshot(paths);
        QCOMPARE(res.snapshot.status, Snapshot::Status::NoActiveProfile);
        QCOMPARE(res.warnings.size(), 0);
    }
    void malformedState() {
        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        const auto paths = pathsUnder(dir.path(), dir.path());
        QVERIFY(QDir().mkpath(paths.configDir));
        QVERIFY(writeFile(paths.stateFile(), R"({ "schemaVersion": })"));

        auto res = loadSnapshot(paths);
        QCOMPARE(res.snapshot.status, Snapshot::Status::StateError);
        QCOMPARE(res.warnings.size(), 1);
        QCOMPARE(res.warnings[0].where, "state");
    }
    void missingProfile() {
        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        const auto paths = pathsUnder(dir.path(), dir.path());
        QVERIFY(QDir().mkpath(paths.profilesDir()));
        QVERIFY(writeFile(paths.stateFile(), R"({ "schemaVersion": 1, "activeProfile": "x.json" })"));

        auto res = loadSnapshot(paths);
        QCOMPARE(res.snapshot.status, Snapshot::Status::ProfileError);
        QCOMPARE(res.warnings.size(), 1);
        QCOMPARE(res.warnings[0].where, "profile");
        QCOMPARE(res.warnings[0].kind, Warning::Kind::Unreadable);
    }
    void rejectedProfile() {
        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        const auto paths = pathsUnder(dir.path(), dir.path());
        QVERIFY(QDir().mkpath(paths.profilesDir()));
        QVERIFY(writeFile(paths.stateFile(), R"({ "schemaVersion": 1, "activeProfile": "x.json" })"));
        QVERIFY(writeFile(paths.profilesDir() + "/x.json", R"({ "schemaVersion": 2, "stickers": [] })"));

        auto res = loadSnapshot(paths);
        QCOMPARE(res.snapshot.status, Snapshot::Status::ProfileError);
        QCOMPARE(res.warnings.size(), 1);
        QCOMPARE(res.warnings[0].effect, WarningEffect::FileRejected);
    }
    void brokenProfile() {
        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        const auto paths = pathsUnder(dir.path(), dir.path());
        QVERIFY(QDir().mkpath(paths.profilesDir()));
        QVERIFY(writeFile(paths.stateFile(), R"({ "schemaVersion": 1, "activeProfile": "x.json" })"));
        QVERIFY(writeFile(paths.profilesDir() + "/x.json", R"({ schemaVersion: "2" })" ));

        auto res = loadSnapshot(paths);
        QCOMPARE(res.snapshot.status, Snapshot::Status::ProfileError);
        QCOMPARE(res.warnings.size(), 1);
        QCOMPARE(res.warnings[0].where, "profile");
        QCOMPARE(res.warnings[0].kind, Warning::Kind::InvalidJson);
    }
    void validProfile() {
        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        const auto paths = pathsUnder(dir.path(), dir.path());
        QVERIFY(QDir().mkpath(paths.profilesDir()));
        QVERIFY(writeFile(paths.stateFile(), R"({ "schemaVersion": 1, "activeProfile": "x.json" })"));
        QVERIFY(writeFile(paths.profilesDir() + "/x.json",
            R"({ "schemaVersion": 1, "stickers": 
            [ { "id": "sticker1", "source": "stPath1" }, { "id": "sticker2", "source": "stPath2" } ] })"));

        auto res = loadSnapshot(paths);
        QCOMPARE(res.snapshot.status, Snapshot::Status::Ok);
        QVERIFY(res.profilePath.has_value());
        QCOMPARE(res.profilePath.value(), paths.profilesDir() + "/x.json");
        QCOMPARE(res.snapshot.stickers.size(), 2);
        QCOMPARE(res.snapshot.stickers[0].id, "sticker1");
        QCOMPARE(res.snapshot.stickers[0].path, paths.stickersDir() + "/stPath1");
        QCOMPARE(res.snapshot.stickers[1].id, "sticker2");
        QCOMPARE(res.snapshot.stickers[0].path, paths.stickersDir() + "/stPath1");
        QCOMPARE(res.warnings.size(), 0);
    }
    void badStickerSkipped() {
        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        const auto paths = pathsUnder(dir.path(), dir.path());
        QVERIFY(QDir().mkpath(paths.profilesDir()));
        QVERIFY(writeFile(paths.stateFile(), R"({ "schemaVersion": 1, "activeProfile": "x.json" })"));
        QVERIFY(writeFile(paths.profilesDir() + "/x.json",
            R"({ "schemaVersion": 1, "stickers": 
            [ { "id": "sticker1", "source": "stPath1" }, { "id": 123, "source": "stPath2" } ] })"));

        auto res = loadSnapshot(paths);
        QCOMPARE(res.snapshot.status, Snapshot::Status::Ok);
        QVERIFY(res.profilePath.has_value());
        QCOMPARE(res.profilePath.value(), paths.profilesDir() + "/x.json");
        QCOMPARE(res.snapshot.stickers.size(), 1);
        QCOMPARE(res.snapshot.stickers[0].id, "sticker1");
        QCOMPARE(res.snapshot.stickers[0].path, paths.stickersDir() + "/stPath1");
        QCOMPARE(res.warnings.size(), 1);
        QCOMPARE(res.warnings[0].effect, WarningEffect::StickerSkipped);
    }
    void directoryProfile() {
        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        const auto paths = pathsUnder(dir.path(), dir.path());
        QVERIFY(QDir().mkpath(paths.profilesDir() + "/directoryTo"));
        QVERIFY(writeFile(paths.stateFile(), R"({ "schemaVersion": 1, "activeProfile": "directoryTo" })"));

        auto res = loadSnapshot(paths);
        QCOMPARE(res.snapshot.status, Snapshot::Status::ProfileError);
        QCOMPARE(res.warnings.size(), 1);
        QCOMPARE(res.warnings[0].kind, Warning::Kind::Unreadable);
    }
    void switchProfile() {
        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        const auto paths = pathsUnder(dir.path(), dir.path());
        QVERIFY(QDir().mkpath(paths.profilesDir()));
        QVERIFY(writeFile(paths.stateFile(), R"({ "schemaVersion": 1, "activeProfile": "good.json" })"));
        QVERIFY(writeFile(paths.profilesDir() + "/good.json",
            R"({ "schemaVersion": 1, "stickers": 
            [ { "id": "stickerX1", "source": "stPath1" }, { "id": "stickerX2", "source": "stPath2" } ] })"));
        QVERIFY(writeFile(paths.profilesDir() + "/bad.json", R"({ schemaVersion: 2 })"));

        auto res = loadSnapshot(paths);
        QCOMPARE(res.snapshot.status, Snapshot::Status::Ok);
        QVERIFY(res.profilePath.has_value());
        QCOMPARE(res.profilePath.value(), paths.profilesDir() + "/good.json");
        QCOMPARE(res.snapshot.stickers.size(), 2);
        QCOMPARE(res.snapshot.stickers[0].id, "stickerX1");
        QCOMPARE(res.snapshot.stickers[1].id, "stickerX2");
        QCOMPARE(res.warnings.size(), 0);

        QVERIFY(writeFile(paths.stateFile(), R"({ "schemaVersion": 1, "activeProfile": "bad.json" })"));

        auto res2 = loadSnapshot(paths);
        QCOMPARE(res2.snapshot.status, Snapshot::Status::ProfileError);
        QCOMPARE(res2.snapshot.stickers.size(), 0);
        QCOMPARE(res2.warnings.size(), 1);
        QCOMPARE(res2.warnings[0].kind, Warning::Kind::InvalidJson);
    }
};

QTEST_APPLESS_MAIN(LoadSnapshotTest)
#include "test_load_snapshot.moc"