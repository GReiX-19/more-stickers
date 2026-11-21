#include "appDirs.hpp"
#include "helpers.hpp"
#include <QTemporaryDir>
#include <QFileInfo>
#include <QTest>

using namespace mrst;
using namespace mrst::tests;

class DirsTest : public QObject {
    Q_OBJECT;

private slots:
    void everythingFromNothing() {
        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        const auto paths = pathsUnder(dir.path(), dir.path());

        QString error;
        QVERIFY(ensureAppDirs(paths, &error));

        QVERIFY(QFileInfo(paths.profilesDir()).isDir());
        QVERIFY(QFileInfo(paths.stickersDir()).isDir());
    }
    void idempotentCalledTwice() {
        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        const auto paths = pathsUnder(dir.path(), dir.path());

        QString error;
        QVERIFY(ensureAppDirs(paths, &error));

        QVERIFY(QFileInfo(paths.profilesDir()).isDir());
        QVERIFY(QFileInfo(paths.stickersDir()).isDir());

        QVERIFY(ensureAppDirs(paths, &error));

        QVERIFY(QFileInfo(paths.profilesDir()).isDir());
        QVERIFY(QFileInfo(paths.stickersDir()).isDir());
    }
    void pathIsFile() {
        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        const QString blocker = dir.filePath("blocker");
        QVERIFY(writeFile(blocker, "x"));

        const auto paths = pathsUnder(blocker, dir.path());

        QString error;
        QVERIFY(!ensureAppDirs(paths, &error));
        QVERIFY(error.contains(paths.profilesDir()));
    }
    void errorNullptr() {
        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        const auto paths = pathsUnder(dir.path(), dir.path());

        QVERIFY(ensureAppDirs(paths, nullptr));

        QVERIFY(QFileInfo(paths.profilesDir()).isDir());
        QVERIFY(QFileInfo(paths.stickersDir()).isDir());
    }
};

QTEST_APPLESS_MAIN(DirsTest)
#include "test_dirs.moc"