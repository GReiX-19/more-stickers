#include <QString>
#include <QByteArray>
#include <QFile>
#include <QIODevice>

bool writeFile(const QString& _path, const QByteArray& _content) {
        QFile f(_path);
        return f.open(QIODevice::WriteOnly | QIODevice::Truncate)
            and f.write(_content) == _content.size();
}