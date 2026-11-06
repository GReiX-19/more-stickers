#pragma once

#include <QString>

namespace mrst {

    enum class WarningEffect {
        StickerSkipped,
        FileRejected,
        ValueAdjusted
    };
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
        WarningEffect effect;
    };
}