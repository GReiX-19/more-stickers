#include "SnapshotWatcher.hpp"
#include "helpers.hpp"
#include "state.hpp"
#include "stateSaver.hpp"
#include "profileSaver.hpp"
#include <QTemporaryDir>
#include <QDir>
#include <QFile>
#include <QSignalSpy>
#include <QTest>

using namespace mrst;
using namespace mrst::tests;

class TestSnapshotWatcher : public QObject {
    Q_OBJECT;

private slots:
    void firstStart() {
        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        const auto paths = pathsUnder(dir.path(), dir.path());
        QVERIFY(QDir().mkpath(paths.profilesDir()));

        SnapshotWatcher watcher{ paths, 5 };
        QSignalSpy spySnap(&watcher, &SnapshotWatcher::snapshotChanged);
        QSignalSpy spyWarn(&watcher, &SnapshotWatcher::warningsChanged);

        watcher.start();

        QCOMPARE(spySnap.count(), 1);
        QCOMPARE(spyWarn.count(), 0);
        QCOMPARE(watcher.current().snapshot.status, Snapshot::Status::NoActiveProfile);
    }
    void inPlaceProfileEdit() {
        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        const auto paths = pathsUnder(dir.path(), dir.path());
        QVERIFY(QDir().mkpath(paths.profilesDir()));

        QString error;
        QVERIFY(saveStateFile(paths.stateFile(), { 1, "x.json" }, &error));
        QVERIFY(saveProfileFile(paths.profilePath("x.json"), makeProfile("x.json", { makeSticker("st1", "stPath1") }), &error));

        SnapshotWatcher watcher{ paths, 5 };
        QSignalSpy spySnap(&watcher, &SnapshotWatcher::snapshotChanged);
        QSignalSpy spyWarn(&watcher, &SnapshotWatcher::warningsChanged);

        watcher.start();

        QCOMPARE(spySnap.count(), 1);
        QCOMPARE(spyWarn.count(), 0);
        QCOMPARE(watcher.current().snapshot.status, Snapshot::Status::Ok);
        QCOMPARE(watcher.current().snapshot.stickers.size(), 1);
        QCOMPARE(watcher.current().snapshot.stickers[0].id, "st1");

        QVERIFY(writeFile(paths.profilesDir() + "/x.json",
            R"({ "schemaVersion": 1, "stickers": [ { "id": "someSticker", "source": "stPath1" } ] })"));

        QTRY_COMPARE(spySnap.count(), 2);
        QCOMPARE(spyWarn.count(), 0);
        QCOMPARE(watcher.current().snapshot.status, Snapshot::Status::Ok);
        QCOMPARE(watcher.current().snapshot.stickers.size(), 1);
        QCOMPARE(watcher.current().snapshot.stickers[0].id, "someSticker");
    }
    void brokenStickerOneWarning() {
        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        const auto paths = pathsUnder(dir.path(), dir.path());
        QVERIFY(QDir().mkpath(paths.profilesDir()));

        QString error;
        QVERIFY(saveStateFile(paths.stateFile(), { 1, "x.json" }, &error));
        QVERIFY(writeFile(paths.profilesDir() + "/x.json",
            R"({ "schemaVersion": 1, "stickers": [ { "id": 123, "source": "stPath1" } ] })"));

        SnapshotWatcher watcher{ paths, 5 };
        QSignalSpy spySnap(&watcher, &SnapshotWatcher::snapshotChanged);
        QSignalSpy spyWarn(&watcher, &SnapshotWatcher::warningsChanged);

        watcher.start();

        QCOMPARE(spySnap.count(), 1);
        QCOMPARE(spyWarn.count(), 1);

        QCOMPARE(watcher.current().snapshot.stickers.size(), 0);
    }
    void warningsChangeWithoutSnapshotChange() {
        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        const auto paths = pathsUnder(dir.path(), dir.path());
        QVERIFY(QDir().mkpath(paths.profilesDir()));

        QString error;
        QVERIFY(saveStateFile(paths.stateFile(), State{ 1, "x.json" }, &error));
        QVERIFY(writeFile(paths.profilePath("x.json"),
            R"({ "schemaVersion": 1, "stickers": [ { "id": "ok", "source": "ok.png" }, { "id": "bad", "source": 1 } ] })"));

        SnapshotWatcher watcher{ paths, 5 };
        QSignalSpy spySnap(&watcher, &SnapshotWatcher::snapshotChanged);
        QSignalSpy spyWarn(&watcher, &SnapshotWatcher::warningsChanged);
        watcher.start();

        QCOMPARE(spySnap.count(), 1);
        QCOMPARE(spyWarn.count(), 1);

        QVERIFY(writeFile(paths.profilePath("x.json"),
            R"({ "schemaVersion": 1, "stickers": [ { "id": "ok", "source": "ok.png" }, { "source": "bad.png" } ] })"));

        QTRY_COMPARE(spyWarn.count(), 2);
        QCOMPARE(spySnap.count(), 1);
    }
    void atomicReplaceTwice() {
        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        const auto paths = pathsUnder(dir.path(), dir.path());
        QVERIFY(QDir().mkpath(paths.profilesDir()));

        QString error;
        QVERIFY(saveStateFile(paths.stateFile(), State{ 1, "x.json" }, &error));
        QVERIFY(saveProfileFile(paths.profilePath("x.json"), makeProfile("x.json", { makeSticker("fromX", "x.png") }), &error));
        QVERIFY(saveProfileFile(paths.profilePath("y.json"), makeProfile("y.json", { makeSticker("fromY", "y.png") }), &error));

        SnapshotWatcher watcher{ paths, 5 };
        QSignalSpy spySnap(&watcher, &SnapshotWatcher::snapshotChanged);
        QSignalSpy spyWarn(&watcher, &SnapshotWatcher::warningsChanged);

        watcher.start();

        QCOMPARE(spySnap.count(), 1);
        QCOMPARE(spyWarn.count(), 0);
        QCOMPARE(watcher.current().snapshot.stickers.size(), 1);
        QCOMPARE(watcher.current().snapshot.stickers[0].id, "fromX");

        QVERIFY(saveStateFile(paths.stateFile(), State{ 1, "y.json" }, &error));

        QTRY_COMPARE(spySnap.count(), 2);
        QCOMPARE(spyWarn.count(), 0);
        QCOMPARE(watcher.current().snapshot.stickers.size(), 1);
        QCOMPARE(watcher.current().snapshot.stickers[0].id, "fromY");

        QVERIFY(saveStateFile(paths.stateFile(), State{ 1, "x.json" }, &error));

        QTRY_COMPARE(spySnap.count(), 3);
        QCOMPARE(spyWarn.count(), 0);
        QCOMPARE(watcher.current().snapshot.stickers.size(), 1);
        QCOMPARE(watcher.current().snapshot.stickers[0].id, "fromX");
    }
    void atomicProfileReplaceTwice() {
        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        const auto paths = pathsUnder(dir.path(), dir.path());
        QVERIFY(QDir().mkpath(paths.profilesDir()));

        QString error;
        QVERIFY(saveStateFile(paths.stateFile(), State{ 1, "x.json" }, &error));
        QVERIFY(saveProfileFile(paths.profilePath("x.json"), makeProfile("x.json", { makeSticker("v1", "a.png") }), &error));

        SnapshotWatcher watcher{ paths, 5 };
        QSignalSpy spySnap(&watcher, &SnapshotWatcher::snapshotChanged);
        watcher.start();
        QCOMPARE(spySnap.count(), 1);

        QVERIFY(saveProfileFile(paths.profilePath("x.json"), makeProfile("x.json", { makeSticker("v2", "a.png") }), &error));
        QTRY_COMPARE(spySnap.count(), 2);
        QCOMPARE(watcher.current().snapshot.stickers[0].id, "v2");

        QVERIFY(saveProfileFile(paths.profilePath("x.json"), makeProfile("x.json", { makeSticker("v3", "a.png") }), &error));
        QTRY_COMPARE(spySnap.count(), 3);
        QCOMPARE(watcher.current().snapshot.stickers[0].id, "v3");
    }
    void stateCreatedAfterStart() {
        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        const auto paths = pathsUnder(dir.path(), dir.path());
        QVERIFY(QDir().mkpath(paths.profilesDir()));

        SnapshotWatcher watcher{ paths, 5 };
        QSignalSpy spySnap(&watcher, &SnapshotWatcher::snapshotChanged);
        QSignalSpy spyWarn(&watcher, &SnapshotWatcher::warningsChanged);

        watcher.start();

        QCOMPARE(spySnap.count(), 1);
        QCOMPARE(spyWarn.count(), 0);
        QVERIFY(writeFile(paths.stateFile(), R"({ "schemaVersion": 1, "activeProfile": "x.json" })"));
        QTRY_COMPARE(spySnap.count(), 2);
        QCOMPARE(spyWarn.count(), 1);
        QCOMPARE(watcher.current().snapshot.status, Snapshot::Status::ProfileError);
    }
    void profileDeletedThenRecreated() {
        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        const auto paths = pathsUnder(dir.path(), dir.path());
        QVERIFY(QDir().mkpath(paths.profilesDir()));

        QString error;
        QVERIFY(saveStateFile(paths.stateFile(), State{ 1, "x.json" }, &error));
        QVERIFY(saveProfileFile(paths.profilePath("x.json"), makeProfile("x.json", { makeSticker("st1", "a.png") }), &error));

        SnapshotWatcher watcher{ paths, 5 };
        QSignalSpy spySnap(&watcher, &SnapshotWatcher::snapshotChanged);
        QSignalSpy spyWarn(&watcher, &SnapshotWatcher::warningsChanged);
        watcher.start();
        QCOMPARE(watcher.current().snapshot.status, Snapshot::Status::Ok);

        QVERIFY(QFile::remove(paths.profilePath("x.json")));
        QTRY_COMPARE(spySnap.count(), 2);
        QCOMPARE(watcher.current().snapshot.status, Snapshot::Status::ProfileError);
        QCOMPARE(spyWarn.count(), 1);

        QVERIFY(saveProfileFile(paths.profilePath("x.json"), makeProfile("x.json", { makeSticker("st1", "a.png") }), &error));
        QTRY_COMPARE(spySnap.count(), 3);
        QCOMPARE(watcher.current().snapshot.status, Snapshot::Status::Ok);
        QCOMPARE(spyWarn.count(), 2);
    }
    void editingOldAndNewProfiles() {
        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        const auto paths = pathsUnder(dir.path(), dir.path());
        QVERIFY(QDir().mkpath(paths.profilesDir()));

        QString error;
        QVERIFY(saveStateFile(paths.stateFile(), State{ 1, "x.json" }, &error));
        QVERIFY(saveProfileFile(paths.profilePath("x.json"), makeProfile("x.json", { makeSticker("fromX", "x.png") }), &error));
        QVERIFY(saveProfileFile(paths.profilePath("y.json"), makeProfile("y.json", { makeSticker("fromY", "y.png") }), &error));

        SnapshotWatcher watcher{ paths, 5 };
        QSignalSpy spySnap(&watcher, &SnapshotWatcher::snapshotChanged);
        QSignalSpy spyWarn(&watcher, &SnapshotWatcher::warningsChanged);

        watcher.start();

        QCOMPARE(spySnap.count(), 1);
        QCOMPARE(spyWarn.count(), 0);
        QCOMPARE(watcher.current().snapshot.stickers.size(), 1);
        QCOMPARE(watcher.current().snapshot.stickers[0].id, "fromX");

        QVERIFY(saveStateFile(paths.stateFile(), State{ 1, "y.json" }, &error));
        QTRY_COMPARE(spySnap.count(), 2);
        QCOMPARE(spyWarn.count(), 0);
        QCOMPARE(watcher.current().snapshot.stickers.size(), 1);
        QCOMPARE(watcher.current().snapshot.stickers[0].id, "fromY");

        QVERIFY(saveProfileFile(paths.profilePath("x.json"), makeProfile("x.json", { makeSticker("fromX", "x_changed.png") }), &error));

        QTest::qWait(100);
        QCOMPARE(spySnap.count(), 2);
        QCOMPARE(spyWarn.count(), 0);

        QVERIFY(saveProfileFile(paths.profilePath("y.json"), makeProfile("y.json", { makeSticker("fromY", "y_changed.png") }), &error));

        QTRY_COMPARE(spySnap.count(), 3);
        QCOMPARE(spyWarn.count(), 0);
    }
    void identicalEdit() {
        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        const auto paths = pathsUnder(dir.path(), dir.path());
        QVERIFY(QDir().mkpath(paths.profilesDir()));

        QString error;
        QVERIFY(saveStateFile(paths.stateFile(), State{ 1, "x.json" }, &error));
        QVERIFY(saveProfileFile(paths.profilePath("x.json"), makeProfile("x.json", { makeSticker("st1", "stPath1") }), &error));

        SnapshotWatcher watcher{ paths, 5 };
        QSignalSpy spySnap(&watcher, &SnapshotWatcher::snapshotChanged);
        QSignalSpy spyWarn(&watcher, &SnapshotWatcher::warningsChanged);

        watcher.start();

        QCOMPARE(spySnap.count(), 1);
        QCOMPARE(spyWarn.count(), 0);
        QCOMPARE(watcher.current().snapshot.stickers.size(), 1);
        QCOMPARE(watcher.current().snapshot.stickers[0].id, "st1");

        QVERIFY(saveProfileFile(paths.profilePath("x.json"), makeProfile("x.json", { makeSticker("st1", "stPath1") }), &error));

        QTest::qWait(100);
        QCOMPARE(spySnap.count(), 1);
        QCOMPARE(spyWarn.count(), 0);
    }
    void bunchOfEdits() {
        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        const auto paths = pathsUnder(dir.path(), dir.path());
        QVERIFY(QDir().mkpath(paths.profilesDir()));

        QString error;
        QVERIFY(saveStateFile(paths.stateFile(), State{ 1, "x.json" }, &error));
        QVERIFY(saveProfileFile(paths.profilePath("x.json"), makeProfile("x.json", { makeSticker("st1", "stPath1") }), &error));

        SnapshotWatcher watcher{ paths, 5 };
        QSignalSpy spySnap(&watcher, &SnapshotWatcher::snapshotChanged);
        QSignalSpy spyWarn(&watcher, &SnapshotWatcher::warningsChanged);

        watcher.start();

        QCOMPARE(spySnap.count(), 1);
        QCOMPARE(spyWarn.count(), 0);
        QCOMPARE(watcher.current().snapshot.stickers.size(), 1);
        QCOMPARE(watcher.current().snapshot.stickers[0].id, "st1");

        QVERIFY(writeFile(paths.profilesDir() + "/x.json",
            R"({ "schemaVersion": 1, "stickers": [ { "id": "st2", "source": "stPath2" } ] })"));
        QVERIFY(writeFile(paths.profilesDir() + "/x.json",
            R"({ "schemaVersion": 1, "stickers": [ { "id": "st3", "source": "stPath3" } ] })"));
        QVERIFY(writeFile(paths.profilesDir() + "/x.json",
            R"({ "schemaVersion": 1, "stickers": [ { "id": "st4", "source": "stPath4" } ] })"));

        QTRY_COMPARE(spySnap.count(), 2);
        QTest::qWait(100);
        QCOMPARE(spySnap.count(), 2);
        QCOMPARE(spyWarn.count(), 0);
        QCOMPARE(watcher.current().snapshot.stickers.size(), 1);
        QCOMPARE(watcher.current().snapshot.stickers[0].id, "st4");
    }
};

QTEST_GUILESS_MAIN(TestSnapshotWatcher)
#include "test_snapshot_watcher.moc"