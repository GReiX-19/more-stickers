#include "ConfigStore.hpp"
#include "profileLoader.hpp"

namespace mrst {
    ConfigStore::ConfigStore(QString _path, QObject* _parent)
        : QObject(_parent), m_path(std::move(_path)) {
    }
    void ConfigStore::reload() {
        LoadResult res = loadProfileFile(m_path);
        
        m_warnings = std::move(res.warnings);

        if (res.profile)
            m_profile = std::move(res.profile);

        emit changed();
    }
} // namespace mrst