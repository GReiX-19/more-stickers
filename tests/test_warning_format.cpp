#include "warningFormat.hpp"
#include <QTest>

using namespace mrst;

class WarningFormatTest : public QObject {
    Q_OBJECT;

private slots:
    void specExample() {
        Warning w{ .where = "stickers[2] (id: \"cat\")",
            .key = "opacity", .message = "expected number, found string \"high\"",
            .effect = WarningEffect::StickerSkipped };

        QCOMPARE(formatWarning(w), R"(stickers[2] (id: "cat"): "opacity" expected number, found string "high" - sticker skipped)");
    }
    void missingId() {
        Warning w{ .where = "stickers[1]", .key = "id", .message = "is a required value",
            .effect = WarningEffect::StickerSkipped };

        QCOMPARE(formatWarning(w), R"(stickers[1]: "id" is a required value - sticker skipped)");
    }
    void emptyKey() {
        Warning w{ .where = "stickers[0]", .message = "expected object, found number \"5\"",
            .effect = WarningEffect::StickerSkipped };

        QCOMPARE(formatWarning(w), R"(stickers[0]: expected object, found number "5" - sticker skipped)");
    }
    void clamped() {
        Warning w{ .where = "stickers[4] (id: \"flower\")", .key = "opacity", .message = "= 20 was clamped",
            .effect = WarningEffect::ValueAdjusted };

        QCOMPARE(formatWarning(w), R"(stickers[4] (id: "flower"): "opacity" = 20 was clamped - value adjusted)");
    }
    void stateLevel() {
        Warning w{ .where = "state", .key = "activeProfile", .message = "must be a plain file name (no '/', not '.' or '..')",
            .effect = WarningEffect::FileRejected };

        QCOMPARE(formatWarning(w), R"(state: "activeProfile" must be a plain file name (no '/', not '.' or '..') - file rejected)");
    }
    void profileLevel() {
        Warning w{ .where = "profile", .key = "schemaVersion", .message = "must be only \"1\"",
            .effect = WarningEffect::FileRejected };

        QCOMPARE(formatWarning(w), R"(profile: "schemaVersion" must be only "1" - file rejected)");
    }
};

QTEST_APPLESS_MAIN(WarningFormatTest)
#include "test_warning_format.moc"