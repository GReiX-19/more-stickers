#pragma once

#include "appPaths.hpp"
#include "snapshotLoader.hpp"
#include "Warning.hpp"
#include <QObject>
#include <QFileSystemWatcher>
#include <QTimer>
#include <QString>
#include <optional>

namespace mrst {
    class SnapshotWatcher : public QObject {
        Q_OBJECT;

    public:
        explicit SnapshotWatcher(AppPaths _paths, int _debounceMs = 50, QObject* _parent = nullptr);

        void start(); // Emits snapshotChanged() synchronously, so connect to the signals before calling start().
        const SnapshotResult& current() const { return m_current; }

    signals:
        void snapshotChanged();
        void warningsChanged();

    private:
        void onWatchEvent();
        void reload();
        void rearmWatches();
        void moveProfileWatch(SnapshotResult& _newSnapshot);

    private:
        AppPaths m_paths;
        QFileSystemWatcher m_watcher;
        QTimer m_debounce;
        SnapshotResult m_current;
        std::optional<Snapshot> m_lastEmitted;
        std::optional<QString> m_watchedProfile;
        QList<Warning> m_lastWarnings;
    };
} // namespace mrst