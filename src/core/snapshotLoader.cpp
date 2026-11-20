#include "snapshotLoader.hpp"
#include "activeProfile.hpp"
#include "profileLoader.hpp"
#include "appPaths.hpp"

namespace mrst {
    SnapshotResult loadSnapshot(const AppPaths& _paths) {
        SnapshotResult res;

        auto active = resolveActiveProfile(_paths);
        res.warnings = std::move(active.warnings);

        LoadResult prof;
        if (active.status == ActiveProfile::Status::Ready) {
            prof = loadProfileFile(*active.path);
            res.warnings.append(prof.warnings);
            res.profilePath = *active.path;
        }

        res.snapshot = buildSnapshot(active.status, prof.profile, _paths.stickersDir());

        return res;
    }
} // namespace mrst