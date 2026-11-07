#include "profileLoader.hpp"
#include <QTemporaryDir>
#include <QtTest>

using namespace mrst;

class LoadProfileTest : public QObject {
    Q_OBJECT;

private slots:
    void validFile() {
        const QString path = QFINDTESTDATA("test_configs/valid_config.json");
        QVERIFY2(!path.isEmpty(), "fixture not found");

        auto res = loadProfileFile(path);

        QVERIFY(res.profile.has_value());
        QCOMPARE(res.warnings.size(), 0);
        QCOMPARE(res.profile->schemaVersion, 1);
        QCOMPARE(res.profile->name, "validConfig");
        QCOMPARE(res.profile->stickers.size(), 1);
        QCOMPARE(res.profile->stickers[0].id, "stickerId");
        QCOMPARE(res.profile->stickers[0].source, "someStickerPath");
    }
    void missingFile() {
        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        const QString path = dir.filePath("does_not_exist.json");

        auto res = loadProfileFile(path);

        QCOMPARE(res.profile.has_value(), false);
        QCOMPARE(res.warnings.size(), 1);
        QCOMPARE(res.warnings[0].where, "profile");
        QCOMPARE(res.warnings[0].kind, Warning::Kind::Unreadable);
        QCOMPARE(res.warnings[0].effect, WarningEffect::FileRejected);
    }
    void emptyFile() {
        const QString path = QFINDTESTDATA("test_configs/empty_config.json");
        QVERIFY2(!path.isEmpty(), "fixture not found");
        
        auto res = loadProfileFile(path);

        QCOMPARE(res.profile.has_value(), false);
        QCOMPARE(res.warnings.size(), 1);
        QCOMPARE(res.warnings[0].where, "profile");
        QCOMPARE(res.warnings[0].kind, Warning::Kind::InvalidJson);
        QCOMPARE(res.warnings[0].effect, WarningEffect::FileRejected);
    }
    void validProfileSkippedSticker() {
        const QString path = QFINDTESTDATA("test_configs/valid_with_broken_sticker.json");
        QVERIFY2(!path.isEmpty(), "fixture not found");
        
        auto res = loadProfileFile(path);

        QVERIFY(res.profile.has_value());
        QCOMPARE(res.warnings.size(), 1);
        QCOMPARE(res.warnings[0].where, "stickers[1]");
        QCOMPARE(res.warnings[0].key, "id");
        QCOMPARE(res.warnings[0].kind, Warning::Kind::Missing);
        QCOMPARE(res.warnings[0].effect, WarningEffect::StickerSkipped);
        QCOMPARE(res.profile->schemaVersion, 1);
        QCOMPARE(res.profile->name, "validWithBrokenSticker");
        QCOMPARE(res.profile->stickers.size(), 1);
        QCOMPARE(res.profile->stickers[0].id, "stickerId");
        QCOMPARE(res.profile->stickers[0].source, "someStickerPath");
    }
};

QTEST_APPLESS_MAIN(LoadProfileTest)
#include "test_load_profile.moc"