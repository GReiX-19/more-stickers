#include "warningFormat.hpp"

namespace mrst {
    namespace {
        QString effectToString(WarningEffect _effect) {
            switch (_effect) {
            case WarningEffect::StickerSkipped: return QString{ " - sticker skipped" };
            case WarningEffect::FileRejected: return QString{ " - file rejected" };
            case WarningEffect::ValueAdjusted: return QString{ " - value adjusted" };
            }
            return QString{};
        }
    }

    QString formatWarning(const Warning& _w) {
        QString fw;

        fw += _w.where + ":";
        if (!_w.key.isEmpty()) fw += " \"" + _w.key + "\"";
        fw += " " + _w.message;
        fw += effectToString(_w.effect);

        fw.remove('\n');
        return fw;
    }
}