#include "snapshotWriter.hpp"
#include "snapshotEncoder.hpp"

namespace mrst {
    bool writeSnapshotLine(QIODevice& _out, const Snapshot& _snap) {
        if (!_out.isWritable())
            return false;

        const QByteArray line = encodeSnapshot(_snap) + '\n';
        return _out.write(line) == line.size();
    }
} // namespace mrst