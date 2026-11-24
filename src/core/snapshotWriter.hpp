#pragma once

#include <QIODevice>
#include "snapshot.hpp"

namespace mrst {
    bool writeSnapshotLine(QIODevice& _out, const Snapshot& _snap); // _out must already be open for writing
} // namespace mrst