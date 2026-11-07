#pragma once

#include <QObject>
#include <QList>
#include <QString>
#include <optional>
#include "profileJson.hpp"
#include "Warning.hpp"

namespace mrst {
    class ConfigStore : public QObject {
    Q_OBJECT

    public:
        explicit ConfigStore(QString _path, QObject* _parent = nullptr);

        void reload();
        const std::optional<Profile>& profile() const { return m_profile; }
        const QList<Warning>& warnings() const { return m_warnings; }
        const QString& path() const { return m_path; }

    signals:
        void changed();

    private:
        QString m_path;
        std::optional<Profile> m_profile;
        QList<Warning> m_warnings;
    };
} //namespace mrst