#include "snapshotEncoder.hpp"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTest>

using namespace mrst;

class SnapshotEncoderTest : public QObject {
    Q_OBJECT;

    static QJsonObject parseObject(const QByteArray& _bytes) {
        return QJsonDocument::fromJson(_bytes).object();
    }
    static SnapshotSticker makeSticker(const QString& _id) {
        SnapshotSticker st{};
        st.id = _id;
        st.path = "/stk/" + _id + ".png";
        return st;
    }

private slots:
    void outputIsSingleLine() {
        Snapshot snap;
        snap.status = Snapshot::Status::Ok;
        snap.profileName = "line\nbreak";
        snap.stickers.append(makeSticker("a\nb"));

        const QByteArray bytes = encodeSnapshot(snap);

        QVERIFY(!bytes.isEmpty());
        QVERIFY(!bytes.contains('\n'));
        QVERIFY(!bytes.contains('\r'));
        QVERIFY(!parseObject(bytes).isEmpty());
    }
    void versionAndStatusStrings() {
        const QList<std::pair<Snapshot::Status, QString>> cases{
            { Snapshot::Status::NoActiveProfile, "no_active_profile" },
            { Snapshot::Status::StateError, "state_error" },
            { Snapshot::Status::ProfileError, "profile_error" },
            { Snapshot::Status::Ok, "ok" }
        };

        for (const auto& [status, text] : cases) {
            Snapshot snap;
            snap.status = status;

            const QJsonObject obj = parseObject(encodeSnapshot(snap));

            QVERIFY2(!obj.isEmpty(), qPrintable(text));
            QCOMPARE(obj.value("v").toInt(), kProtocolVersion);
            QCOMPARE(obj.value("status").toString(), text);
            QVERIFY(obj.value("stickers").isArray());
            QCOMPARE(obj.value("stickers").toArray().size(), 0);
        }
    }
    void monitorOnlyWrittenWhenSet() {
        Snapshot snap;
        snap.status = Snapshot::Status::Ok;
        SnapshotSticker without = makeSticker("a");
        SnapshotSticker with = makeSticker("b");
        with.monitor = "DP-1";
        snap.stickers = { without, with };

        const QJsonArray arr = parseObject(encodeSnapshot(snap)).value("stickers").toArray();

        QCOMPARE(arr.size(), 2);
        QVERIFY(!arr[0].toObject().contains("monitor"));
        QVERIFY(arr[1].toObject().contains("monitor"));
        QCOMPARE(arr[1].toObject().value("monitor").toString(), "DP-1");
    }
    void roundTripKeepsEveryField() {
        Snapshot snap;
        snap.status = Snapshot::Status::Ok;
        snap.profileName = "work";

        SnapshotSticker st = makeSticker("a");
        st.x = 10;
        st.y = 20;
        st.sizeMode = SizeMode::Scale;
        st.width = 300;
        st.height = 400;
        st.scale = 2.5;
        st.rotation = 45;
        st.opacity = 0.5;
        snap.stickers = { st, makeSticker("b") };

        const QJsonObject obj = parseObject(encodeSnapshot(snap));

        QVERIFY(!obj.isEmpty());
        QCOMPARE(obj.value("profile").toString(), "work");

        const QJsonArray arr = obj.value("stickers").toArray();
        QCOMPARE(arr.size(), 2);

        const QJsonObject first = arr[0].toObject();
        QCOMPARE(first.value("id").toString(), "a");
        QCOMPARE(first.value("url").toString(), "file:///stk/a.png");
        QCOMPARE(first.value("x").toDouble(), 10.0);
        QCOMPARE(first.value("y").toDouble(), 20.0);
        QCOMPARE(first.value("sizeMode").toString(), "scale");
        QCOMPARE(first.value("width").toDouble(), 300.0);
        QCOMPARE(first.value("height").toDouble(), 400.0);
        QCOMPARE(first.value("scale").toDouble(), 2.5);
        QCOMPARE(first.value("rotation").toDouble(), 45.0);
        QCOMPARE(first.value("opacity").toDouble(), 0.5);

        QCOMPARE(arr[1].toObject().value("id").toString(), "b");
        QCOMPARE(arr[1].toObject().value("sizeMode").toString(), "size");
    }
    void shieldedCharacters_data() {
        QTest::addColumn<QString>("source");

        QTest::newRow("cat #1") << "cat #1";
        QTest::newRow("100%") << "100%";
        QTest::newRow("  ") << "  ";
        QTest::newRow("€£¥∑π∞") << "€£¥∑π∞";
    }
    void shieldedCharacters() {
        QFETCH(QString, source);

        Snapshot snap;
        snap.status = Snapshot::Status::Ok;
        snap.profileName = "prof";
        snap.stickers.append(makeSticker(source));

        const QJsonObject obj = parseObject(encodeSnapshot(snap));

        const QJsonArray arr = obj.value("stickers").toArray();
        QCOMPARE(arr.size(), 1);
        const QJsonValue url = arr[0].toObject().value("url");
        QVERIFY(url.isString());
        QCOMPARE(QUrl(url.toString()).toLocalFile(), snap.stickers[0].path);
    }
};

QTEST_APPLESS_MAIN(SnapshotEncoderTest)
#include "test_snapshot_encoder.moc"