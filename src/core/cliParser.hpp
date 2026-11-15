#pragma once

#include <QString>
#include <QStringList>
#include <variant>

namespace mrst {
    struct SetProfileCommand { QString name; };
    struct CliError { QString message; };

    std::variant<SetProfileCommand, CliError> parseCommandLine(const QStringList& _args);
} // namespace mrst