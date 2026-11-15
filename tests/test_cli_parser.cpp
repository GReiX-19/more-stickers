#include "cliParser.hpp"
#include <QTest>

using namespace mrst;

class CliParserTest : public QObject {
    Q_OBJECT;

private slots:
    void validParse() {
        auto res = parseCommandLine({ "sticker-config", "--set-profile", "x.json" });
        QVERIFY2(std::holds_alternative<SetProfileCommand>(res),
         qPrintable(std::get_if<CliError>(&res) ? std::get<CliError>(res).message : QString{}));
        QCOMPARE(std::get<SetProfileCommand>(res).name, "x.json");
    }
    void secondValidParse() {
        auto res = parseCommandLine({ "sticker-config", "--set-profile=x.json" });
        QVERIFY2(std::holds_alternative<SetProfileCommand>(res),
         qPrintable(std::get_if<CliError>(&res) ? std::get<CliError>(res).message : QString{}));
        QCOMPARE(std::get<SetProfileCommand>(res).name, "x.json");
    }
    void noArgs() {
        auto res = parseCommandLine({"sticker-config"});
        QVERIFY(std::holds_alternative<CliError>(res));
        QVERIFY(!std::get<CliError>(res).message.isEmpty());
    }
    void unknownArg() {
        auto res = parseCommandLine({ "sticker-config", "--set-profile-timeout", "x.json" });
        QVERIFY(std::holds_alternative<CliError>(res));
        QVERIFY(!std::get<CliError>(res).message.isEmpty());
    }
    void lastArg() {
        auto res = parseCommandLine({ "sticker-config", "--set-profile" });
        QVERIFY(std::holds_alternative<CliError>(res));
        QVERIFY(!std::get<CliError>(res).message.isEmpty());
    }
    void extraPositionalArgs() {
        auto res = parseCommandLine({ "sticker-config", "--set-profile", "x.json", "y.json" });
        QVERIFY(std::holds_alternative<CliError>(res));
        QVERIFY(!std::get<CliError>(res).message.isEmpty());
    }
    void parserDoesNotValidateName() {
        auto res = parseCommandLine({ "sticker-config", "--set-profile", "../x.json" });
        QVERIFY2(std::holds_alternative<SetProfileCommand>(res),
         qPrintable(std::get_if<CliError>(&res) ? std::get<CliError>(res).message : QString{}));
        QCOMPARE(std::get<SetProfileCommand>(res).name, "../x.json");
    }
};

QTEST_APPLESS_MAIN(CliParserTest)
#include "test_cli_parser.moc"