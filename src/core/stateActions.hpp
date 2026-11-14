#pragma once

#include <QString>
#include <optional>

namespace mrst {
    struct AppPaths;

    std::optional<QString> setActiveProfile(const AppPaths& _paths, const QString& _name);
} // namespace mrst