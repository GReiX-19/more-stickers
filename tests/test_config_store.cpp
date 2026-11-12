#include "ConfigStore.hpp"
#include "Warning.hpp"
#include <QFile>
#include <QIODevice>
#include <QTemporaryDir>
#include <QByteArray>
#include <QSignalSpy>
#include <QtTest>

using namespace mrst;

class ConfigStoreTest : public QObject {
    Q_OBJECT;

    static bool writeFile(const QString& _path, const QByteArray& _content) {
        QFile f(_path);
        return f.open(QIODevice::WriteOnly | QIODevice::Truncate)
            and f.write(_content) == _content.size();
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

        QVERIFY(writeFile(path, good1));
        store.reload();
        QVERIFY(store.profile().has_value());
        QCOMPARE(store.profile()->name, "one");
        QCOMPARE(store.warnings().size(), 0);

        QVERIFY(writeFile(path, broken));
        store.reload();
        QVERIFY(store.profile().has_value());
        QCOMPARE(store.profile()->name, "one");
        QCOMPARE(store.warnings().size(), 1);
        QCOMPARE(store.warnings()[0].kind, Warning::Kind::InvalidJson);

        QVERIFY(writeFile(path, good2));
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

        QVERIFY(writeFile(path, broken));
        store.reload();
        QCOMPARE(store.profile().has_value(), false);
        QCOMPARE(store.warnings().size(), 1);
        QCOMPARE(store.warnings()[0].kind, Warning::Kind::InvalidJson);
        QCOMPARE(store.warnings()[0].effect, WarningEffect::FileRejected);

        QCOMPARE(spy.count(), 1);
    }
    void missingFile() {
        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        const QString path = dir.filePath("does_not_exist.json");
        ConfigStore store{ path };
        QSignalSpy spy(&store, &ConfigStore::changed);

        store.reload();
        QCOMPARE(store.profile().has_value(), false);
        QCOMPARE(store.warnings().size(), 1);
        QCOMPARE(store.warnings()[0].kind, Warning::Kind::Unreadable);
        QCOMPARE(store.warnings()[0].effect, WarningEffect::FileRejected);

        QCOMPARE(spy.count(), 1);
    }
    void deletedFile() {
        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        const QString path = dir.filePath("profile.json");
        const QByteArray good = R"({ "schemaVersion": 1, "name": "one", "stickers": [] })";

        ConfigStore store{ path };
        QSignalSpy spy(&store, &ConfigStore::changed);

        QVERIFY(writeFile(path, good));
        store.reload();
        QVERIFY(store.profile().has_value());
        QCOMPARE(store.warnings().size(), 0);
        QCOMPARE(store.profile()->schemaVersion, 1);
        QCOMPARE(store.profile()->name, "one");

        QVERIFY(QFile::remove(path));
        store.reload();
        QVERIFY(store.profile().has_value());
        QCOMPARE(store.warnings().size(), 1);
        QCOMPARE(store.warnings()[0].kind, Warning::Kind::Unreadable);
        QCOMPARE(store.warnings()[0].effect, WarningEffect::FileRejected);

        QCOMPARE(spy.count(), 2);
    }
};

QTEST_GUILESS_MAIN(ConfigStoreTest)
#include "test_config_store.moc"