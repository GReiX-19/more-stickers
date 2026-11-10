#include "appPaths.hpp"
#include <QtEnvironmentVariables>
#include <QtTest>

using namespace mrst;

class PathsTest : public QObject {
    Q_OBJECT;

private slots:
    void layoutUnderBases() {
        const auto paths = pathsUnder("/cfg", "/data");

        QCOMPARE(paths.configDir, "/cfg/more-stickers");
        QCOMPARE(paths.dataDir, "/data/more-stickers");
        QCOMPARE(paths.profilesDir(), "/cfg/more-stickers/profiles");
        QCOMPARE(paths.stateFile(), "/cfg/more-stickers/state.json");
        QCOMPARE(paths.stickersDir(), "/data/more-stickers/stickers");
    }
    void trailingSlashIgnored() {
        const auto a = pathsUnder("/cfg", "/data");
        const auto b = pathsUnder("/cfg/", "/data/");

        QCOMPARE(a.configDir, b.configDir);
        QCOMPARE(a.dataDir, b.dataDir);
    }
};

QTEST_APPLESS_MAIN(PathsTest)
#include "test_paths.moc"