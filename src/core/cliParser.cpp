#include "cliParser.hpp"
#include <QCommandLineParser>
#include <QCommandLineOption>

namespace mrst {
    std::variant<SetProfileCommand, PathsCommand, GuiCommand, CliError> parseCommandLine(const QStringList& _args) {
        QCommandLineOption setActive("set-profile", "Set chosen profile to active.", "file");
        QCommandLineOption paths("paths", "Prints out default paths.");
        QCommandLineParser parser;
        parser.addOption(setActive);
        parser.addOption(paths);

        if (!parser.parse(_args))
            return CliError{ parser.errorText() };
        if (!parser.positionalArguments().isEmpty())
            return CliError{ "Unexpected argument: " + parser.positionalArguments()[0] };

        const bool hasSet = parser.isSet(setActive);
        const bool hasPaths = parser.isSet(paths);

        if (hasSet and hasPaths)
            return CliError{ "Options --set-profile and --paths cannot be used together." };
        if (hasPaths)
            return PathsCommand{};
        if (hasSet)
            return SetProfileCommand{ parser.value(setActive) };
        return GuiCommand{};
    }
} // namespace mrst