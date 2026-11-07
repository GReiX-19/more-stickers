#include "ConfigStore.hpp"
#include <QFile>
#include <QIODevice>
#include <QTemporaryDir>
#include <QByteArray>
#include <QSignalSpy>
#include <QtTest>

using namespace mrst;

class ConfigStoreTest : public QObject {
    Q_OBJECT;

    static void writeFile(const QString& _path, const QByteArray& _content) {
        QFile f(_path);
        QVERIFY(f.open(QIODevice::WriteOnly | QIODevice::Truncate));
        f.write(_content);
    }

private slots:
    void validBrokenValid() {
        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        const QString path = dir.filePath("profile.json");

        const QByteArray good1 =
            R"({ "schemaVersion": 1, "name": "one", "stickers": [] })";
        const QByteArray broken = R"({ "schemaVersion": })";
        const QByteArray good2 =
            R"({ "schemaVersion": 1, "name": "two", "stickers": [] })";

        ConfigStore store{ path };
        QSignalSpy spy(&store, &ConfigStore::changed);

        writeFile(path, good1);
        store.reload();
        QVERIFY(store.profile().has_value());
        QCOMPARE(store.profile()->name, "one");
        QCOMPARE(store.warnings().size(), 0);

        writeFile(path, broken);
        store.reload();
        QVERIFY(store.profile().has_value());
        QCOMPARE(store.profile()->name, "one");
        QCOMPARE(store.warnings().size(), 1);
        QCOMPARE(store.warnings()[0].kind, Warning::Kind::InvalidJson);

        writeFile(path, good2);
        store.reload();
        QVERIFY(store.profile().has_value());
        QCOMPARE(store.profile()->name, "two");
        QCOMPARE(store.warnings().size(), 0);

        QCOMPARE(spy.count(), 3);
    }
    void brokenProfile() {
        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        const QString path = dir.filePath("profile.json");
        const QByteArray broken = R"({ "schemaVersion": })";

        ConfigStore store{ path };
        QSignalSpy spy(&store, &ConfigStore::changed);

        writeFile(path, broken);
        store.reload();
        QCOMPARE(store.profile().has_value(), false);
        QCOMPARE(store.warnings().size(), 1);
        QCOMPARE(store.warnings()[0].kind, Warning::Kind::InvalidJson);
        QCOMPARE(store.warnings()[0].effect, WarningEffect::FileRejected);

        QCOMPARE(spy.count(), 1);
    }
    void missingFile() {
        const QString path{"test_config/missing_config.json"};
        ConfigStore store{ path };
        QSignalSpy spy(&store, &ConfigStore::changed);

        store.reload();
        QCOMPARE(store.profile().has_value(), false);
        QCOMPARE(store.warnings().size(), 1);
        QCOMPARE(store.warnings()[0].kind, Warning::Kind::Unreadable);
        QCOMPARE(store.warnings()[0].effect, WarningEffect::FileRejected);

        QCOMPARE(spy.count(), 1);
    }
};

QTEST_GUILESS_MAIN(ConfigStoreTest)
#include "test_config_store.moc"