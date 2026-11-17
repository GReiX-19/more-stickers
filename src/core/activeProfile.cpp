#include "activeProfile.hpp"
#include "appPaths.hpp"
#include "stateLoader.hpp"
#include <algorithm>

namespace mrst {
    bool rejectsFile(const QList<Warning>& _warnings) {
        return std::ranges::any_of(_warnings,
            [](const Warning& warn) { return warn.effect == WarningEffect::FileRejected; });
    }
    ActiveProfile resolveActiveProfile(const AppPaths& _paths) {
        ActiveProfile profile;
        auto res = loadStateFile(_paths.stateFile());

        profile.warnings = std::move(res.warnings);
        if (!res.state.has_value()) {
            if (rejectsFile(profile.warnings))
                profile.status = ActiveProfile::Status::Rejected;
            else
                profile.status = ActiveProfile::Status::NoState;
            return profile;
        }
        
        profile.status = ActiveProfile::Status::Ready;
        profile.path = _paths.profilePath(res.state->activeProfile);
        return profile;
    }
}