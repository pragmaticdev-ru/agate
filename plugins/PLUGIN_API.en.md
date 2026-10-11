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

## Cache and performance

- A conversion result is cached in `%LOCALAPPDATA%\agate\plugin-cache` and reused
  when the same version of the file is opened again.
- The cache limit is configured in Settings (1 GiB by default; `0` = unbounded).
- The first open pays the conversion cost; reopening is instant.

## Errors

If a conversion fails, the plugin returns an error, crashes or times out, the
canvas shows the “could not open file” screen with the reason. The file is not
deleted and keeps synchronizing; a plugin error is not a sync error.
