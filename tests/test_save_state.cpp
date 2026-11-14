#include "stateSaver.hpp"
#include "stateLoader.hpp"
#include <QTemporaryDir>
#include <QByteArray>
#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <QScopeGuard>
#include <QtTest>

using namespace mrst;

class SaveStateTest : public QObject {
    Q_OBJECT;

    static bool writeFile(const QString& _path, const QByteArray& _content) {
        QFile f(_path);
        return f.open(QIODevice::WriteOnly | QIODevice::Truncate)
            and f.write(_content) == _content.size();
    }
    static QByteArray readFile(const QString& _path) {
        QFile f(_path);
        return f.open(QIODevice::ReadOnly) ? f.readAll() : QByteArray{};
    }

private slots:
    void loadChangeSaveLoad() {
        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        const QString path = dir.filePath("valid_state.json");
        const QByteArray text = R"({ "schemaVersion": 1, "activeProfile": "x.json" })";
        QVERIFY(writeFile(path, text));

        auto res1 = loadStateFile(path);

        QVERIFY(res1.state.has_value());
        QCOMPARE(res1.warnings.size(), 0);
        res1.state->activeProfile = "y.json";
        QString error;
        QVERIFY(saveStateFile(path, res1.state.value(), &error));

        auto res2 = loadStateFile(path);

        QVERIFY(res2.state.has_value());
        QCOMPARE(res2.warnings.size(), 0);
        QCOMPARE(res2.state->schemaVersion, 1);
        QCOMPARE(res2.state->activeProfile, "y.json");
    }
    void saveIntoMissingFolder() {
        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        const QString folder = dir.filePath("no_such_folder");
        const QString path = folder + "/state.json";

        State state;
        state.schemaVersion = 1;
        state.activeProfile = "x.json";

        QString error;
        QVERIFY(!saveStateFile(path, state, &error));
        QVERIFY(!error.isEmpty());
        QVERIFY(!QFileInfo::exists(path));
        QVERIFY(!QDir(folder).exists());
    }
    void saveDefaultState() {
        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        const QString path = dir.filePath("state.json");

        State state;
        QString error;
        QVERIFY(!saveStateFile(path, state, &error));
        QVERIFY(error.contains("schemaVersion"));
        QVERIFY(error.contains("activeProfile"));
        QVERIFY(!QFileInfo::exists(path));
    }
    void nullErrorPointer() {
        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        const QString path = dir.filePath("state.json");

        QVERIFY(!saveStateFile(path, State{}, nullptr));
        QVERIFY(!QFileInfo::exists(path));
        QVERIFY(saveStateFile(path, State{ 1, "x.json" }, nullptr));
        QVERIFY(QFileInfo::exists(path));
    }
    void staleErrorStringIgnored() {
        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        const QString path = dir.filePath("state.json");

        QString error = "left over from an earlier call";
        QVERIFY(saveStateFile(path, State{ 1, "x.json" }, &error));
        QVERIFY(QFileInfo::exists(path));
    }
    void invalidStateKeepsExistingFile_data() {
        QTest::addColumn<int>("version");
        QTest::addColumn<QString>("name");

        QTest::newRow("bad version") << 2 << QString("x.json");
        QTest::newRow("empty name") << 1 << QString("");
        QTest::newRow("parent traversal") << 1 << QString("../x.json");
        QTest::newRow("subfolder") << 1 << QString("sub/x.json");
        QTest::newRow("single dot") << 1 << QString(".");
        QTest::newRow("double dot") << 1 << QString("..");
    }
    void invalidStateKeepsExistingFile() {
        QFETCH(int, version);
        QFETCH(QString, name);

        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        const QString path = dir.filePath("state.json");
        const QByteArray text = R"({ "schemaVersion": 1, "activeProfile": "x.json" })";
        QVERIFY(writeFile(path, text));

        QString error;
        QVERIFY(!saveStateFile(path, State{ version, name }, &error));
        QVERIFY(!error.isEmpty());
        QCOMPARE(readFile(path), text);
    }
    void failedSaveOldFileUntouched() {
        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        const QString path = dir.filePath("valid.json");
        const QByteArray text = R"({ "schemaVersion": 1, "activeProfile": "x.json" })";
        QVERIFY(writeFile(path, text));

        const auto originalPerms = QFile::permissions(dir.path());
        auto restore = qScopeGuard([p = dir.path(), originalPerms] {
            QFile::setPermissions(p, originalPerms);
        });

        QVERIFY(QFile::setPermissions(dir.path(), QFileDevice::ReadOwner | QFileDevice::ExeOwner));
        QFile probe(dir.filePath("probe"));
        if (probe.open(QIODevice::WriteOnly))
            QSKIP("permissions are not enforced here (running as root?)");

        QString error;
        QVERIFY(!saveStateFile(path, State{ 1, "y.json" }, &error));
        QVERIFY(!error.isEmpty());
        QCOMPARE(readFile(path), text);
    }
};

QTEST_APPLESS_MAIN(SaveStateTest)
#include "test_save_state.moc"