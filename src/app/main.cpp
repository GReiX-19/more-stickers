#include "cliParser.hpp"
#include "stateActions.hpp"
#include "appPaths.hpp"
#include <QCoreApplication>
#include <QStringList>
#include <QTextStream>
#include <cstdlib>

namespace {
    int fail(const QString& _msg) {
        QTextStream(stderr) << _msg << '\n';
        return EXIT_FAILURE;
    }
}

int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);
    const QStringList args = QCoreApplication::arguments();

    auto res = mrst::parseCommandLine(args);

    if (std::holds_alternative<mrst::CliError>(res)) {
        return fail(std::get<mrst::CliError>(res).message);
    }
    if (std::holds_alternative<mrst::SetProfileCommand>(res)) {
        auto err = mrst::setActiveProfile(mrst::defaultPaths(), std::get <mrst::SetProfileCommand>(res).name);
        if (err.has_value()) {
            return fail(err.value());
        }
    }

    return EXIT_SUCCESS;
}