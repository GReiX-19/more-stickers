#include "cliParser.hpp"
#include "stateActions.hpp"
#include "appPaths.hpp"
#include "formatPaths.hpp"
#include <QCoreApplication>
#include <QStringList>
#include <QTextStream>
#include <cstdlib>
#include <variant>

namespace {
    int fail(const QString& _msg) {
        QTextStream(stderr) << _msg << '\n';
        return EXIT_FAILURE;
    }

    template<class... Ts>
    struct overloaded : Ts... {
        using Ts::operator()...;
    };
    template<class... Ts>
    overloaded(Ts...) -> overloaded<Ts...>;
}

int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);
    const auto res = mrst::parseCommandLine(QCoreApplication::arguments());

    return std::visit(overloaded{
        [](const mrst::CliError& _err) {
            return fail(_err.message);
        },
        [](const mrst::SetProfileCommand& _cmd) {
            if (const auto err = mrst::setActiveProfile(mrst::defaultPaths(), _cmd.name))
                return fail(err.value());
            return EXIT_SUCCESS;
        },
        [](const mrst::PathsCommand& _cmd) {
            QTextStream(stdout) << mrst::formatPaths(mrst::defaultPaths());
            return EXIT_SUCCESS;
        },
        [](const mrst::GuiCommand& _cmd) {
            QTextStream(stdout) << "Config application window will be available some day maybe idk bruh\n";
            return EXIT_SUCCESS;
        }
        }, res);
}