#pragma once

#include <QString>

namespace mrst {
    struct Warning {
        enum class Kind {
            Missing,
            WrongType,
            Duplicate,
            Clamped,
            Empty,
            InvalidValue,
            OutOfRange,
        };
        QString where;
        QString key;
        QString message;
        Kind kind;
    };
}