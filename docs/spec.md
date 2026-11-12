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
| `x`, `y` | number | no | `0` each | logical pixels from the top-left of the screen |
| `sizeMode` | `"size"` or `"scale"` | no | `"size"` |
| `width`, `height` | number | no | `200` each | read when `sizeMode` is `"size"` |
| `scale` | number | no | `1.0` | read when `sizeMode` is `"scale"` |
| `rotation` | number | no | `0` | degrees |
| `opacity` | number | no | `1` | clamped to 0..1 with a warning | 

### State file

| Key | Type | Required | Default |
| :--- | :--- | :--- | :--- |
| `schemaVersion` | integer | yes | none (must be `1`) |
| `activeProfile` | string | yes | none |

`activeProfile` must be a plain file name: non-empty, containing no `/`, and not exactly `.` or `..`. Valid: `work.json`, `.hidden.json`. Invalid: `../x.json`, `sub/x.json`, `/x.json`, `..`.

**State file behaviors:**
| Situation | Behavior |
| :--- | :--- |
| Absent | no active profile, no warnings |
| Unreadable, invalid `activeProfile`, permissions denied, malformed JSON, bad schemaVersion | no state, appropriate warnings (when the configuration app is opened); overlay shows nothing, logs the problem |
| Present | valid state, no warnings |
| Unknown keys | ignored while parsing |
| State valid, no profile | overlay shows nothing, logs the problem |

**File layout:**  
- config base - `$XDG_CONFIG_HOME` if `$XDG_CONFIG_HOME` else `~/.config`
- data base - `$XDG_DATA_HOME` if `$XDG_DATA_HOME` else `~/.local/share`
- config dir - config base + `/more-stickers`
- data dir - data base + `/more-stickers`
- profiles - config dir + `/profiles`
- state file - config dir + `/state.json`
- stickers folder - data dir + `/stickers`

### Validation

| Situation | Behavior |
| :--- | :--- |
| Invalid JSON, wrong top-level shape, missing or unsupported `schemaVersion` | whole file rejected, keep last good state, warn |
| Missing required field, or any wrong-typed field in a `sticker` | skip that sticker, warn |
| Duplicate `id` | skip the later one, warn |
| `opacity` out of range | clamp, warn |
| Unknown key | ignore, but preserve on write |
| Missing optional field | use default |

**Warning format:** `stickers[2] (id "cat"): "opacity" expected number, found string "high" - sticker skipped`

**Command:** `sticker-config --set-profile <file>` checks only that the file exists. On failure it prints to stderr, exits non-zero, and leaves the state file untouched.