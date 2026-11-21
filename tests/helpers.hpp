#pragma once

#include <QString>
#include <QByteArray>
#include <QFile>
#include <QFileDevice>
#include <QDir>
#include <QIODevice>
#include <QList>
#include "sticker.hpp"
#include "profileJson.hpp"

namespace mrst {
    namespace tests {
        inline bool writeFile(const QString& _path, const QByteArray& _content) {
            QFile f(_path);
            return f.open(QIODevice::WriteOnly | QIODevice::Truncate)
                and f.write(_content) == _content.size();
        }
        inline QByteArray readFile(const QString& _path) {
            QFile f(_path);
            return f.open(QIODevice::ReadOnly) ? f.readAll() : QByteArray{};
        }
        inline Sticker makeSticker(const QString& _id, const QString& _source, bool _enabled = true) {
            Sticker st{};
            st.id = _id;
            st.source = _source;
            st.enabled = _enabled;
            return st;
        }
        inline Profile makeProfile(const QString& _name, const QList<Sticker>& _stickers) {
            Profile prof{};
            prof.schemaVersion = 1;
            prof.name = _name;
            prof.stickers = _stickers;
            return prof;
        }

        class ReadOnlyDir {
            QString m_dir;
            QFileDevice::Permissions m_originalPerms;
            bool m_enforced;
        public:
            ReadOnlyDir(const ReadOnlyDir&) = delete;
            ReadOnlyDir& operator=(const ReadOnlyDir&) = delete;

            explicit ReadOnlyDir(const QString& _dir)
                : m_dir(_dir), m_originalPerms(QFile::permissions(_dir)), m_enforced(false) {

                if (!QFile::setPermissions(_dir, QFileDevice::ReadOwner | QFileDevice::ExeOwner))
                    return;

                QFile probe(QDir(_dir).filePath("probe"));
                if (probe.open(QIODevice::WriteOnly)) {
                    probe.remove();
                    return;
                }
                m_enforced = true;
            }
            ~ReadOnlyDir() {
                QFile::setPermissions(m_dir, m_originalPerms);
            }
            bool enforced() const {
                return m_enforced;
            }
        };
    }
}