# agate converter plugins — API v1

> **Warning.** agate loads and executes any DLL found in the `plugins/`
> directory with your user's privileges. That means arbitrary code execution.
> Install only plugins you trust.

## What a plugin does

A plugin is a dynamic library (`.dll`, Windows x64) that:

1. declares the input file extensions agate should synchronize;
2. on request, converts such a file into a system format (PDF or a raster image)
   that agate can show on the canvas.

The original file is never modified and keeps being synchronized as a normal
document version.

## Directory and discovery

- Put the DLL into the `plugins/` directory next to `agate.exe` (or the directory
  configured under `[plugins] dir` in `config.toml`).
- Plugins are discovered **only at application start**. Adding or removing a DLL
  takes effect after a restart.
- Extensions from loaded plugins extend the built-in set. Files with those
  extensions found in connected areas are imported automatically.

## ABI v1

The sole exported symbol:

```c
__declspec(dllexport) const AgatePluginV1* agate_plugin_v1(void);
```

The full structure layout is in `agate_plugin.h`. Rules:

- `abi_version` = 1;
- `id` is unique among plugins, `[a-z0-9._-]{1,64}`;
- `extension` is lowercase letters/digits, no dot, `[a-z0-9]{1,16}`;
- `output` is one of: `pdf`, `png`, `jpg`, `jpeg`, `webp`, `bmp`, `tif`, `tiff`;
- an extension must not shadow a built-in one and must not be claimed by two
  plugins (the first DLL by file name wins; conflicts are logged);
- a DLL whose descriptor fails validation is disabled entirely.

## The `convert` contract

```c
int32_t convert(const char* src, const char* dst, char* err, size_t err_cap);
```

- returns `0` on success, a non-zero code otherwise;
- on failure, write the reason into `err` as a NUL-terminated UTF-8 string;
- **never let a C++ exception or panic cross the boundary** — that is undefined
  behaviour; catch everything inside the function;
- the function may be called from a thread other than the loader thread;
- `dst` is a path in the temporary directory; create exactly that file.

## Example (C++)

```cpp
#include "agate_plugin.h"
#include <string>

static const AgatePluginFormat kFormats[] = {
    { "dwg", "pdf" },
};

static int32_t convert(const char* src, const char* dst, char* err, size_t cap) {
    try {
        // ... convert src to a PDF at dst ...
        return 0;
    } catch (const std::exception& e) {
        if (cap) { std::snprintf(err, cap, "%s", e.what()); }
        return 1;
    } catch (...) {
        if (cap) { std::snprintf(err, cap, "unknown error"); }
        return 1;
    }
}

static const AgatePluginV1 kDescriptor = {
    AGATE_PLUGIN_ABI_VERSION,
    "example.dwg",
    "Example DWG converter",
    "1.0.0",
    1,
    kFormats,
    &convert,
    nullptr,
};

extern "C" __declspec(dllexport) const AgatePluginV1* agate_plugin_v1(void) {
    return &kDescriptor;
}
```

## Drawing scale in PDF

So the canvas can measure Length/Area in drawing units (mm/m) with a `1:S`
scale, the converter must embed the standard viewport/`Measure` table into the
output PDF (ISO 32000 §12.9). Without it, agate shows values in sheet
centimetres.

For each page, add a viewport with `Measure`:

```
<</Type /Viewport /BBox [x0 y0 x1 y1]
   /Measure <</Type /Measure /Subtype /RL
      /D [<</C 35.28 /F /D /U (mm)>>]     % distances
      /A [<</C 1244.7 /F /D /U (mm2)>>]   % areas
      /X [<</C 35.28 /F /D /U (mm)>>]     % coordinate (compatibility)
      /R (1:100)>> >>
```

- `/D`, `/A` (and the optional `/X`) are number formats: either a single
  dictionary or an array of dictionaries; agate reads both forms and takes the
  first entry with a non-empty `/U`;
- `/C` is the coefficient: how many drawing units per PDF point
  (`1 pt = 1/72 in`); `/F /D` is the decimal format; `/U` is the unit prefix as
  an **ASCII** string (`mm`, `mm2`, not “²”); **an empty `/U ( )` means “no
  scale”** — agate then honestly falls back to sheet centimetres;
- `/R` is the scale text (e.g. `(1:100)`).

Coefficient formula: `C = (25.4/72)·S·k`, where `S` is the scale denominator
(`1:S`) and `k` is drawing units per millimetre:

| Unit | `k` |
|---|---|
| mm | 1 |
| cm | 0.1 |
| m | 0.001 |
| in | 1/25.4 |
| ft | 1/304.8 |

Area: `A = C²` (unit `/U` — `mm2` etc.).

Examples: a drawing in mm at 1:100 → `C = (25.4/72)·100·1 ≈ 35.28`; a drawing
in metres at 1:500 → `C = (25.4/72)·500·0.001 ≈ 0.176`.

The table can be injected with an incremental PDF update (new objects + xref +
trailer with `/Prev`) when the file is not encrypted, not linearised, has no
object streams and uses a classic xref; otherwise rewrite the file fully
(`lopdf` or equivalent). agate reads both number-format forms and treats an
empty `/U` as “no scale”.

## Cache and performance

- A conversion result is cached in `%LOCALAPPDATA%\agate\plugin-cache` and reused
  when the same version of the file is opened again.
- The cache limit is configured in Settings (1 GiB by default; `0` = unbounded).
- The first open pays the conversion cost; reopening is instant.

## Errors

If a conversion fails, the plugin returns an error, crashes or times out, the
canvas shows the “could not open file” screen with the reason. The file is not
deleted and keeps synchronizing; a plugin error is not a sync error.
