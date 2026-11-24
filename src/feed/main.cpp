#include "appDirs.hpp"
#include "SnapshotWatcher.hpp"
#include "snapshotWriter.hpp"
#include "warningFormat.hpp"
#include <QCoreApplication>
#include <QFile>
#include <QTextStream>
#include <QIODevice>
#include <QTimer>
#include <cstdlib>
#include <unistd.h>
#include <csignal>

namespace {
    int fail(const QString& _msg) {
        QTextStream(stderr) << _msg << '\n';
        return EXIT_FAILURE;
    }
}

int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);
    const auto paths = mrst::defaultPaths();

    std::signal(SIGPIPE, SIG_IGN);

    QString error;
    if (!mrst::ensureAppDirs(paths, &error))
        return fail(error);

    QFile out;
    if (!out.open(STDOUT_FILENO, QIODevice::WriteOnly | QIODevice::Unbuffered))
        return fail(out.errorString());

    mrst::SnapshotWatcher watcher(paths);
    QObject::connect(&watcher, &mrst::SnapshotWatcher::snapshotChanged, &app,
        [&] {
            if (!mrst::writeSnapshotLine(out, watcher.current().snapshot)) {
                QTextStream(stderr) << "stdout is closed, exiting..." << '\n';
                QCoreApplication::exit(EXIT_FAILURE);
            }
        });
    QObject::connect(&watcher, &mrst::SnapshotWatcher::warningsChanged, &app,
        [&] {
            for (const auto& w : watcher.current().warnings)
                QTextStream(stderr) << mrst::formatWarning(w) << '\n';
        });

    QTimer::singleShot(0, &app, [&] { watcher.start(); });

    return app.exec();
}