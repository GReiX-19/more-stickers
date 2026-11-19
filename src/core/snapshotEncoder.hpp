#pragma once

#include <QByteArray>
#include "snapshot.hpp"

namespace mrst {
    constexpr int kProtocolVersion = 1;
    QByteArray encodeSnapshot(const Snapshot& _snap);
} // namespace mrst