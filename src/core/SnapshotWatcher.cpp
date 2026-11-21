#include "SnapshotWatcher.hpp"

namespace mrst {
    SnapshotWatcher::SnapshotWatcher(AppPaths _paths, int _debounceMs, QObject* _parent)
        : QObject(_parent), m_paths(std::move(_paths)) {
        m_debounce.setInterval(_debounceMs);
        m_debounce.setSingleShot(true);

        connect(&m_watcher, &QFileSystemWatcher::fileChanged, this, &SnapshotWatcher::onWatchEvent);
        connect(&m_watcher, &QFileSystemWatcher::directoryChanged, this, &SnapshotWatcher::onWatchEvent);
        connect(&m_debounce, &QTimer::timeout, this, &SnapshotWatcher::reload);
    }

    void SnapshotWatcher::start() {
        reload();
    }

    void SnapshotWatcher::onWatchEvent() {
        m_debounce.start();
    }
    void SnapshotWatcher::reload() {
        rearmWatches();
        auto res = loadSnapshot(m_paths);

        moveProfileWatch(res);

        const bool snapshotDiffers = !m_lastEmitted or *m_lastEmitted != res.snapshot;
        const bool warningsDiffer = m_lastWarnings != res.warnings;

        m_current = std::move(res);
        if (snapshotDiffers) {
            m_lastEmitted = m_current.snapshot;
            emit snapshotChanged();
        }
        if (warningsDiffer) {
            m_lastWarnings = m_current.warnings;
            emit warningsChanged();
        }
    }
    void SnapshotWatcher::rearmWatches() {
        QStringList paths{ m_paths.configDir, m_paths.profilesDir(), m_paths.stateFile() };
        if (m_watchedProfile)
            paths.append(*m_watchedProfile);

        m_watcher.addPaths(paths);
    }
    void SnapshotWatcher::moveProfileWatch(SnapshotResult& _newSnapshot) {
        if (_newSnapshot.profilePath != m_watchedProfile) {
            if (m_watchedProfile)
                m_watcher.removePath(*m_watchedProfile);
            m_watchedProfile = _newSnapshot.profilePath;
            if (m_watchedProfile)
                m_watcher.addPath(*m_watchedProfile);
            _newSnapshot = loadSnapshot(m_paths);
        }
    }
} // namespace mrst