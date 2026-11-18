#include "snapshot.hpp"
#include <QJsonValue>
#include <QTest>

using namespace mrst;

class SnapshotTest : public QObject {
    Q_OBJECT;

    static Sticker makeSticker(const QString& _id, const QString& _source, bool _enabled = true) {
        Sticker st{};
        st.id = _id;
        st.source = _source;
        st.enabled = _enabled;
        return st;
    }
    static Profile makeProfile(const QString& _name, const QList<Sticker>& _stickers) {
        Profile prof{};
        prof.schemaVersion = 1;
        prof.name = _name;
        prof.stickers = _stickers;
        return prof;
    }

private slots:
    void noStateGivesNoActiveProfile() {
        auto snap = buildSnapshot(ActiveProfile::Status::NoState, std::nullopt, "/stk");

        QCOMPARE(snap.status, Snapshot::Status::NoActiveProfile);
        QVERIFY(snap.stickers.isEmpty());
        QVERIFY(snap.profileName.isEmpty());
    }
    void rejectedGivesStateError() {
        auto snap = buildSnapshot(ActiveProfile::Status::Rejected, std::nullopt, "/stk");

        QCOMPARE(snap.status, Snapshot::Status::StateError);
        QVERIFY(snap.stickers.isEmpty());
    }
    void readyWithoutProfileGivesProfileError() {
        auto snap = buildSnapshot(ActiveProfile::Status::Ready, std::nullopt, "/stk");

        QCOMPARE(snap.status, Snapshot::Status::ProfileError);
        QVERIFY(snap.stickers.isEmpty());
    }
    void readyWithProfileGivesOk() {
        const auto prof = makeProfile("work", { makeSticker("a", "a.png") });
        auto snap = buildSnapshot(ActiveProfile::Status::Ready, prof, "/stk");

        QCOMPARE(snap.status, Snapshot::Status::Ok);
        QCOMPARE(snap.profileName, "work");
        QCOMPARE(snap.stickers.size(), 1);
        QCOMPARE(snap.stickers[0].id, "a");
    }
    void statusWinsOverProvidedProfile() {
        const auto prof = makeProfile("work", { makeSticker("a", "a.png") });

        auto noState = buildSnapshot(ActiveProfile::Status::NoState, prof, "/stk");
        QCOMPARE(noState.status, Snapshot::Status::NoActiveProfile);
        QVERIFY(noState.stickers.isEmpty());

        auto rejected = buildSnapshot(ActiveProfile::Status::Rejected, prof, "/stk");
        QCOMPARE(rejected.status, Snapshot::Status::StateError);
        QVERIFY(rejected.stickers.isEmpty());
    }
    void disabledStickersAreFiltered() {
        const auto prof = makeProfile("p", {
            makeSticker("a", "a.png"),
            makeSticker("b", "b.png", false),
            makeSticker("c", "c.png")
            });
        auto snap = buildSnapshot(ActiveProfile::Status::Ready, prof, "/stk");

        QCOMPARE(snap.stickers.size(), 2);
        QCOMPARE(snap.stickers[0].id, "a");
        QCOMPARE(snap.stickers[1].id, "c");
    }
    void allDisabledStillOkWithNoStickers() {
        const auto prof = makeProfile("p", { makeSticker("a", "a.png", false) });
        auto snap = buildSnapshot(ActiveProfile::Status::Ready, prof, "/stk");

        QCOMPARE(snap.status, Snapshot::Status::Ok);
        QVERIFY(snap.stickers.isEmpty());
    }
    void orderIsPreserved() {
        const auto prof = makeProfile("p", {
            makeSticker("one", "1.png"),
            makeSticker("two", "2.png"),
            makeSticker("three", "3.png")
            });
        auto snap = buildSnapshot(ActiveProfile::Status::Ready, prof, "/stk");

        QCOMPARE(snap.stickers.size(), 3);
        QCOMPARE(snap.stickers[0].id, "one");
        QCOMPARE(snap.stickers[1].id, "two");
        QCOMPARE(snap.stickers[2].id, "three");
    }
    void pathIsJoinedWithStickersDir() {
        const auto prof = makeProfile("p", { makeSticker("a", "a.png"), makeSticker("b", "sub/b.png") });
        auto snap = buildSnapshot(ActiveProfile::Status::Ready, prof, "/stk");

        QCOMPARE(snap.stickers[0].path, "/stk/a.png");
        QCOMPARE(snap.stickers[1].path, "/stk/sub/b.png");
    }
    void layoutFieldsAreCopied() {
        Sticker st = makeSticker("a", "a.png");
        st.monitor = "DP-1";
        st.x = 10;
        st.y = 20;
        st.sizeMode = SizeMode::Scale;
        st.width = 300;
        st.height = 400;
        st.scale = 2.5;
        st.rotation = 45;
        st.opacity = 0.5;

        auto snap = buildSnapshot(ActiveProfile::Status::Ready, makeProfile("p", { st }), "/stk");

        QCOMPARE(snap.stickers.size(), 1);
        const auto& out = snap.stickers[0];
        QCOMPARE(out.monitor, std::optional<QString>{ "DP-1" });
        QCOMPARE(out.x, 10);
        QCOMPARE(out.y, 20);
        QCOMPARE(out.sizeMode, SizeMode::Scale);
        QCOMPARE(out.width, 300);
        QCOMPARE(out.height, 400);
        QCOMPARE(out.scale, 2.5);
        QCOMPARE(out.rotation, 45);
        QCOMPARE(out.opacity, 0.5);
    }
    void unsetMonitorStaysUnset() {
        const auto prof = makeProfile("p", { makeSticker("a", "a.png") });
        auto snap = buildSnapshot(ActiveProfile::Status::Ready, prof, "/stk");

        QCOMPARE(snap.stickers[0].monitor, std::nullopt);
    }
    void skippedEntriesDoNotAffectSnapshot() {
        auto withSkipped = makeProfile("p", { makeSticker("a", "a.png") });
        withSkipped.skipped.append({ 0, QJsonValue{ 5 } });
        const auto clean = makeProfile("p", { makeSticker("a", "a.png") });

        auto a = buildSnapshot(ActiveProfile::Status::Ready, withSkipped, "/stk");
        auto b = buildSnapshot(ActiveProfile::Status::Ready, clean, "/stk");

        QCOMPARE(a.status, b.status);
        QCOMPARE(a.stickers.size(), b.stickers.size());
        QCOMPARE(a.stickers[0].id, b.stickers[0].id);
        QCOMPARE(a.stickers[0].path, b.stickers[0].path);
    }
};

QTEST_APPLESS_MAIN(SnapshotTest)
#include "test_snapshot.moc"