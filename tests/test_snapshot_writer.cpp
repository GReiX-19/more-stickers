#include "snapshotWriter.hpp"
#include <QBuffer>
#include <QJsonDocument>
#include <QTest>

namespace mrst {
    namespace tests {
        class BrokenDevice : public QIODevice {
        public:
            BrokenDevice() { open(QIODevice::WriteOnly); }
        protected:
            qint64 readData(char*, qint64) override { return -1; }
            qint64 writeData(const char*, qint64) override { return -1; }
        };
    }
}

using namespace mrst;
using namespace mrst::tests;

class SnapshotWriterTest : public QObject {
    Q_OBJECT;

private slots:
    void oneSnapOneLine() {
        QBuffer buf;
        Snapshot snap{ .status = Snapshot::Status::Ok,
            .profileName = "x.json", .stickers = { { .id = "cat", .path = "cat.png" } } };
        
        QVERIFY(buf.open(QIODevice::WriteOnly));
        QVERIFY(writeSnapshotLine(buf, snap));
        QVERIFY(buf.data().endsWith('\n'));
        QVERIFY(QJsonDocument::fromJson(buf.data()).isObject());
        QCOMPARE(QJsonDocument::fromJson(buf.data()).object().value("profile").toString(), "x.json");
    }
    void twoSnapsTwoLines() {
        QBuffer buf;
        Snapshot snap{ .status = Snapshot::Status::Ok,
            .profileName = "x.json", .stickers = { { .id = "cat", .path = "cat.png" } } };
        Snapshot snap2{ .status = Snapshot::Status::ProfileError,
            .profileName = "y.json", .stickers = { { .id = "dog", .path = "dog.png" } } };

        QVERIFY(buf.open(QIODevice::WriteOnly));
        QVERIFY(writeSnapshotLine(buf, snap));
        QVERIFY(writeSnapshotLine(buf, snap2));
        auto lines = buf.data().split('\n');
        QVERIFY(QJsonDocument::fromJson(lines[0]).isObject());
        QCOMPARE(QJsonDocument::fromJson(lines[0]).object().value("status").toString(), "ok");
        QVERIFY(QJsonDocument::fromJson(lines[1]).isObject());
        QCOMPARE(QJsonDocument::fromJson(lines[1]).object().value("status").toString(), "profile_error");
    }
    void failingDevice() {
        BrokenDevice dev;
        QVERIFY(!writeSnapshotLine(dev, Snapshot{}));
    }
};

QTEST_APPLESS_MAIN(SnapshotWriterTest)
#include "test_snapshot_writer.moc"