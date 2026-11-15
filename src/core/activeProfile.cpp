#include "activeProfile.hpp"
#include "appPaths.hpp"
#include "stateLoader.hpp"

namespace mrst {
    ActiveProfile resolveActiveProfile(const AppPaths& _paths) {
        ActiveProfile profile;
        auto res = loadStateFile(_paths.stateFile());

        profile.warnings = std::move(res.warnings);
        if (!res.state.has_value()) 
            return profile;

        profile.path = _paths.profilePath(res.state->activeProfile);
        return profile;
    }
}