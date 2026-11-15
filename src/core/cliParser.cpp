#include "cliParser.hpp"
#include <QCommandLineParser>
#include <QCommandLineOption>

namespace mrst {
    std::variant<SetProfileCommand, CliError> parseCommandLine(const QStringList& _args) {
        QCommandLineOption setActive("set-profile", "Set chosen profile to active.", "file");
        QCommandLineParser parser;
        parser.addOption(setActive);

        if (!parser.parse(_args)) 
            return CliError{ parser.errorText() };
        if (!parser.positionalArguments().isEmpty())
            return CliError{ "Unexpected argument: " + parser.positionalArguments()[0] };
        if (!parser.isSet(setActive))
            return CliError{ "Missing required option --set-profile <file>." };
            
        return SetProfileCommand{ parser.value(setActive) };
    }
} // namespace mrst