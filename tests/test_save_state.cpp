#include "stateSaver.hpp"
#include "stateLoader.hpp"
#include <QTemporaryDir>
#include <QByteArray>
#include <QFileInfo>
#include <QDir>
#include <QtTest>

using namespace mrst;

class SaveStateTest : public QObject {
    Q_OBJECT;

    static bool writeFile(const QString& _path, const QByteArray& _content) {
        QFile f(_path);
        return f.open(QIODevice::WriteOnly | QIODevice::Truncate)
            and f.write(_content) == _content.size();
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
        QCOMPARE(saveStateFile(path, state, &error), false);
        QVERIFY(!error.isEmpty());
        QVERIFY(!QFileInfo::exists(path));
        QVERIFY(!QDir(folder).exists());
    }
};

QTEST_APPLESS_MAIN(SaveStateTest)
#include "test_save_state.moc"