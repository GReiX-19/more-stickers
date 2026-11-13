#include "profileLoader.hpp"
#include "Warning.hpp"
#include <QTemporaryDir>
#include <QFile>
#include <QtTest>

using namespace mrst;

class LoadProfileTest : public QObject {
    Q_OBJECT;

    static bool writeFile(const QString& _path, const QByteArray& _content) {
        QFile f(_path);
        return f.open(QIODevice::WriteOnly | QIODevice::Truncate)
            and f.write(_content) == _content.size();
    }

private slots:
    void validFile() {
        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        const QString path = dir.filePath("valid.json");
        const QByteArray text = R"({ "name": "validConfig", "schemaVersion": 1, "stickers": [ { "id": "stickerId",
            "source": "someStickerPath" } ] })";
        QVERIFY(writeFile(path, text));

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
        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        const QString path = dir.filePath("empty.json");
        const QByteArray text = "";
        QVERIFY(writeFile(path, text));

        QVERIFY(QFile::exists(path));
        auto res = loadProfileFile(path);

        QCOMPARE(res.profile.has_value(), false);
        QCOMPARE(res.warnings.size(), 1);
        QCOMPARE(res.warnings[0].where, "profile");
        QCOMPARE(res.warnings[0].kind, Warning::Kind::InvalidJson);
        QCOMPARE(res.warnings[0].effect, WarningEffect::FileRejected);
    }
    void validProfileSkippedSticker() {
        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        const QString path = dir.filePath("valid_with_broken_sticker.json");
        const QByteArray text = R"({ "name": "validWithBrokenSticker", "schemaVersion": 1, "stickers": [ {
            "id": "stickerId", "source": "someStickerPath" },{ "source": "someStickerPath" } ] })";
        QVERIFY(writeFile(path, text));
        
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