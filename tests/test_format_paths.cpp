#include "formatPaths.hpp"
#include <QTest>

using namespace mrst;

class FormatPathsTest : public QObject {
    Q_OBJECT;

private slots:
    void formattedPaths() {
        const auto paths = pathsUnder("/cfg", "/data");
        const auto formatted = formatPaths(paths);

        QVERIFY(formatted.contains("Configs: /cfg/more-stickers\n"));
        QVERIFY(formatted.contains("Data: /data/more-stickers\n"));
        QVERIFY(formatted.contains("Profiles: /cfg/more-stickers/profiles\n"));
        QVERIFY(formatted.contains("State file: /cfg/more-stickers/state.json\n"));
        QVERIFY(formatted.contains("Stickers: /data/more-stickers/stickers\n"));
    }
};

QTEST_APPLESS_MAIN(FormatPathsTest)
#include "test_format_paths.moc"