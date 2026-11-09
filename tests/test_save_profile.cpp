#include "profileLoader.hpp"
#include "profileSaver.hpp"
#include <QTemporaryDir>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QIODevice>
#include <QJsonArray>
#include <QJsonDocument>
#include <QtTest>

using namespace mrst;

class SaveProfileTest : public QObject {
    Q_OBJECT;

    static bool writeFile(const QString& _path, const QByteArray& _content) {
        QFile f(_path);
        return f.open(QIODevice::WriteOnly | QIODevice::Truncate)
            and f.write(_content) == _content.size();
    }
    static QString copyFixture(const QTemporaryDir& _dir, const QString& _name) {
        const QString src = QFINDTESTDATA("test_configs/" + _name);
        const QString dst = _dir.filePath(_name);
        return (!src.isEmpty() and QFile::copy(src, dst)) ? dst : QString{};
    }
    static QJsonObject readJsonFile(const QString& _path) {
        QFile f(_path);
        if (!f.open(QIODevice::ReadOnly))
            return {};
        return QJsonDocument::fromJson(f.readAll()).object();
    }

private slots:
    void loadSaveLoad() {
        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        const QString path = copyFixture(dir, "valid_config.json");
        QVERIFY2(!path.isEmpty(), "fixture not found");

        auto res1 = loadProfileFile(path);

        QVERIFY(res1.profile.has_value());
        QCOMPARE(res1.warnings.size(), 0);
        QString error;
        QVERIFY(saveProfileFile(path, res1.profile.value(), &error));

        auto res2 = loadProfileFile(path);

        QVERIFY(res2.profile.has_value());
        QCOMPARE(res2.warnings.size(), 0);
        QCOMPARE(res1.profile->name, res2.profile->name);
        QCOMPARE(res1.profile->schemaVersion, res2.profile->schemaVersion);
        QCOMPARE(res1.profile->stickers[0].id, res2.profile->stickers[0].id);
        QCOMPARE(res1.profile->stickers[0].source, res2.profile->stickers[0].source);
    }
    void unknownKeyTypedSticker() {
        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        const QString path = dir.filePath("profile.json");

        const QByteArray text =
            R"({ "schemaVersion": 1, "stickers": [ { "id": "a", "source": "a.png", "option": "yes" } ] })";

        QVERIFY(writeFile(path, text));

        auto res1 = loadProfileFile(path);

        QVERIFY(res1.profile.has_value());
        QCOMPARE(res1.warnings.size(), 0);
        QString error;
        QVERIFY(saveProfileFile(path, res1.profile.value(), &error));

        auto res2 = loadProfileFile(path);

        QVERIFY(res2.profile.has_value());
        QCOMPARE(res2.warnings.size(), 0);
        QCOMPARE(res2.profile->stickers.size(), 1);
        QVERIFY(res2.profile->stickers[0].raw.contains("option"));
        QCOMPARE(res2.profile->stickers[0].raw.value("option"), "yes");
    }
    void skippedStickerStays() {
        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        const QString path = copyFixture(dir, "valid_with_broken_sticker.json");
        QVERIFY2(!path.isEmpty(), "fixture not found");

        auto res1 = loadProfileFile(path);

        QVERIFY(res1.profile.has_value());
        QCOMPARE(res1.warnings.size(), 1);
        QString error;
        QVERIFY(saveProfileFile(path, res1.profile.value(), &error));

        auto res2 = loadProfileFile(path);

        QVERIFY(res2.profile.has_value());
        QCOMPARE(res2.warnings.size(), 1);
        QCOMPARE(res1.profile->skipped.size(), 1);
        QCOMPARE(res2.profile->skipped.size(), 1);
        QCOMPARE(res1.profile->skipped[0].index, res2.profile->skipped[0].index);
        QCOMPARE(res1.profile->skipped[0].value, res2.profile->skipped[0].value);
    }
    void topLevelUnknownKey() {
        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        const QString path = dir.filePath("profile.json");

        const QByteArray text = R"({ "schemaVersion": 1, "option": "yes", "stickers": [] })";

        QVERIFY(writeFile(path, text));

        auto res1 = loadProfileFile(path);

        QVERIFY(res1.profile.has_value());
        QCOMPARE(res1.warnings.size(), 0);
        QString error;
        QVERIFY(saveProfileFile(path, res1.profile.value(), &error));

        auto res2 = loadProfileFile(path);

        QVERIFY(res2.profile.has_value());
        QCOMPARE(res2.warnings.size(), 0);
        QVERIFY(res2.profile->raw.contains("option"));
    }
    void reorderStickers() {
        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        const QString path = dir.filePath("profile.json");

        const QByteArray text =
            R"({ "schemaVersion": 1, "stickers": 
            [ { "id": "one", "source": "one.png" }, { "id": "two", "source": "two.png" } ] })";

        QVERIFY(writeFile(path, text));

        auto res1 = loadProfileFile(path);

        QVERIFY(res1.profile.has_value());
        QCOMPARE(res1.warnings.size(), 0);
        QCOMPARE(res1.profile->stickers[0].id, "one");
        res1.profile->stickers.swapItemsAt(0, 1);
        QCOMPARE(res1.profile->stickers[0].id, "two");

        QString error;
        QVERIFY(saveProfileFile(path, res1.profile.value(), &error));

        auto res2 = loadProfileFile(path);

        QVERIFY(res2.profile.has_value());
        QCOMPARE(res2.warnings.size(), 0);
        QCOMPARE(res1.profile->stickers[0].id, "two");
        QCOMPARE(res1.profile->stickers[1].id, "one");
    }
    void noDefaults() {
        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        const QString path = copyFixture(dir, "valid_config.json");
        QVERIFY2(!path.isEmpty(), "fixture not found");

        auto res1 = loadProfileFile(path);

        QVERIFY(res1.profile.has_value());
        QCOMPARE(res1.warnings.size(), 0);

        QString error;
        QVERIFY(saveProfileFile(path, res1.profile.value(), &error));

        auto res2 = loadProfileFile(path);

        QVERIFY(res2.profile.has_value());
        QCOMPARE(res2.warnings.size(), 0);
        QVERIFY(!res2.profile->stickers[0].raw.contains("rotation"));
        QVERIFY(!res2.profile->stickers[0].raw.contains("opacity"));
        QVERIFY(!res2.profile->stickers[0].raw.contains("width"));
    }
    void GUIStyleChange() {
        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        const QString path = copyFixture(dir, "valid_config.json");
        QVERIFY2(!path.isEmpty(), "fixture not found");

        auto res1 = loadProfileFile(path);

        QVERIFY(res1.profile.has_value());
        QCOMPARE(res1.warnings.size(), 0);
        res1.profile->stickers[0].x = 42;
        QCOMPARE(res1.profile->stickers[0].x, 42);

        QString error;
        QVERIFY(saveProfileFile(path, res1.profile.value(), &error));

        auto res2 = loadProfileFile(path);

        QVERIFY(res2.profile.has_value());
        QCOMPARE(res2.warnings.size(), 0);
        QCOMPARE(res2.profile->stickers[0].x, 42);
        QCOMPARE(res2.profile->stickers[0].raw.value("x"), 42);
    }
    void skippedAtStart() {
        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        const QString path = dir.filePath("profile.json");

        const QByteArray text = R"({ "schemaVersion": 1, "stickers": [
            { "source": "no_id.png" }, 5, { "id": "a", "source": "a.png" } ] })";
        
        QVERIFY(writeFile(path, text));

        auto res = loadProfileFile(path);

        QVERIFY(res.profile.has_value());
        QCOMPARE(res.profile->stickers.size(), 1);
        QCOMPARE(res.profile->skipped.size(), 2);
        QCOMPARE(res.profile->skipped[0].index, 0);
        QCOMPARE(res.profile->skipped[1].index, 1);

        const QJsonArray before = QJsonDocument::fromJson(text).object().value("stickers").toArray();

        QString error;
        QVERIFY(saveProfileFile(path, res.profile.value(), &error));
        QCOMPARE(readJsonFile(path).value("stickers").toArray(), before);
    }
    void nonObjectElementsStay() {
        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        const QString path = dir.filePath("profile.json");

        const QByteArray text = R"({ "schemaVersion": 1, "stickers": [
            5, { "id": "a", "source": "a.png" }, "text", { "id": "b", "source": "b.png" } ] })";
        
        QVERIFY(writeFile(path, text));

        auto res = loadProfileFile(path);
        
        QVERIFY(res.profile.has_value());
        QCOMPARE(res.profile->stickers.size(), 2);
        QCOMPARE(res.warnings.size(), 2);
        QCOMPARE(res.warnings[0].where, "stickers[0]");
        QCOMPARE(res.warnings[1].where, "stickers[2]");

        const QJsonArray before = QJsonDocument::fromJson(text).object().value("stickers").toArray();

        QString error;
        QVERIFY(saveProfileFile(path, *res.profile, &error));
        QCOMPARE(readJsonFile(path).value("stickers").toArray(), before);
    }
    void saveIntoMissingFolder() {
        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        const QString folder = dir.filePath("no_such_folder");
        const QString path = folder + "/profile.json";

        Profile prof;
        prof.schemaVersion = 1;

        QString error;
        QCOMPARE(saveProfileFile(path, prof, &error), false);
        QVERIFY(!error.isEmpty());
        QVERIFY(!QFileInfo::exists(path));
        QVERIFY(!QDir(folder).exists());
    }
    void failedSaveKeepsOldFile() {
        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        const QString path = dir.filePath("profile.json");
        const QByteArray original = R"({ "schemaVersion": 1, "stickers": [] })";
        QVERIFY(writeFile(path, original));

        Profile prof;
        prof.schemaVersion = 1;
        QVERIFY(saveProfileFile(path, prof, nullptr));
    }
};

QTEST_APPLESS_MAIN(SaveProfileTest)
#include "test_save_profile.moc"