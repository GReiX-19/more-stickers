### Profile file (object)

| Key | Type | Required | Default |
| :--- | :--- | :--- | :--- |
| `schemaVersion` | integer | yes | none (must be `1`) |
| `name` | string | no | `""` |
| `stickers` | array of objects | yes | none |

### Sticker (object)

| Key | Type | Required | Default | Notes |
| :--- | :--- | :--- | :--- | :--- |
| `id` | string | yes | none | non-empty, unique in the file |
| `source` | string | yes | none | non-empty, relative to the stickers folder |
| `enabled` | bool | no | `true` |
| `monitor` | string | no | first screen | named but not connected means hidden, not an error |
| `x`, `y` | number | yes | none | logical pixels from the top-left of the screen |
| `sizeMode` | `"size"` or `"scale"` | no | `"size"` |
| `width`, `height` | number | no | `200` each | read when `sizeMode` is `"size"` |
| `scale` | number | no | `1.0` | read when `sizeMode` is `"scale"` |
| `rotation` | number | no | `0` | degrees |
| `opacity` | number | no | `1` | clamped to 0..1 with a warning | 

**State file (proposed)**: `{ "schemaVersion": 1, "activeProfile": "profile.json" }`, with the filename relative to the profiles folder, written only by app.

### Validation

| Situation | Behaviour |
| :--- | :--- |
| Invalid JSON, wrong top-level shape, missing or unsupported `schemaVersion` | whole file rejected, keep last good state, warn |
| Missing required field, or any wrong-typed field in a `sticker` | skip that sticker, warn |
| Duplicate `id` | stip the later one, warn |
| `opacity` out of range | clamp, warn |
| Uknown key | ignore, but preserve on write |
| Missing optional field | use default |

**Warning format:** `stickers[2] (id "cat"): "opacity" expected number, found string "high" - sticker skipped`

**Command:** `sticker-config --set-profile <file>` checks only that the file exists. On failure it prints to stderr, exits non-zero, and leaves the state file untouched.