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
| `source` | string | yes | none | non-empty, relative path inside the stickers folder, no absolute paths, no `..` escaping, file names may contain any characters; the feed percent-encodes them when building the URL |
| `enabled` | bool | no | `true` |
| `monitor` | string | no | first screen | matches the screen name reported by the compositor (e.g. `eDP-1`, `HDMI-A-2`); unset or empty means the first screen in the compositor's list; named but not connected means hidden, not an error, and the sticker appears when that screen is connected |
| `x`, `y` | number | no | `0` each | logical pixels from the top-left of the screen; position of the top-left corner of the sticker **before** rotation |
| `sizeMode` | `"size"` or `"scale"` | no | `"size"` |
| `width`, `height` | number | no | `200` each | read when `sizeMode` is `"size"` |
| `scale` | number | no | `1.0` | read when `sizeMode` is `"scale"` |
| `rotation` | number | no | `0` | degrees, clockwise, around the center of the sticker; does not change `x`/`y` |
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

### Feed protocol

`sticker-feed` writes one compact JSON object per line to stdout. Each line is a **full snapshot**: the overlay never needs earlier lines.

| Key | Type | Notes |
| :--- | :--- | :--- |
| `v` | integer | protocol version, currently `1` |
| `status` | string | `no_active_profile`, `state_error`, `profile_error` or `ok` |
| `profile` | string | profile name, may be empty |
| `stickers` | array | empty unless `status` is `ok`; disabled stickers are omitted |

Sticker entry: `id`, `url`, `monitor` (only when set), `x`, `y`, `sizeMode`, `width`, `height`, `scale`, `rotation`, `opacity`. `url` is a percent-encoded `file://` URL of the sticker image (this replaced the earlier `path` key). Warnings go to stderr, one per line, in the warning format above.

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
| Invalid JSON, wrong top-level shape, missing or unsupported `schemaVersion` | whole file rejected, show nothing, warn |
| Missing required field, or any wrong-typed field in a `sticker` | skip that sticker, warn |
| Duplicate `id` | skip the later one, warn |
| `opacity` out of range | clamp, warn |
| Unknown key | ignore, but preserve on write |
| Missing optional field | use default |

**Warning format:** `stickers[2] (id: "cat"): "opacity" expected number, found string "high" - sticker skipped`

**Command:** `sticker-config --set-profile <file>` checks only that the file exists. On failure it prints to stderr, exits non-zero, and leaves the state file untouched.

### Overlay behavior

| Situation | Behavior |
| :--- | :--- |
| `status` is not `ok` | show nothing |
| Line is not valid JSON, not an object, or `v` is unsupported | show nothing, log |
| Feed exits (any reason except SIGTERM) | show nothing, log, restart after 500 ms, doubling each failed attempt up to 5 s; the delay resets when a valid snapshot arrives |
| Feed exits on SIGTERM | show nothing, do not restart |
| Feed binary cannot be started | same as a feed exit (restart with backoff) |
| `sizeMode` is `"size"` | image is stretched to `width` x `height`; aspect ratio is not preserved |
| `sizeMode` is `"scale"` | window size is the image's natural size times `scale`; the sticker is hidden until the image has loaded |
| Image fails to load | that sticker is hidden, a warning is logged |
| Rotated sticker | the window is the bounding box of the rotated sticker; it stays click-through |
| Sticker partly outside the screen | shown as is; no clipping handling beyond what the compositor does |

**Known limitations:** sticker files that are added, removed or replaced while the overlay is running are not detected until the overlay restarts; "first screen" is whatever the compositor lists first, which is the built-in display on a typical laptop but not guaranteed.