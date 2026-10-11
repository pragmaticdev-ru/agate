/*
 * agate converter-plugin ABI, version 1 (Windows x64 only).
 *
 * A plugin is a DLL placed in the `plugins/` directory next to `agate.exe`.
 * It exports exactly one symbol:
 *
 *     __declspec(dllexport) const AgatePluginV1* agate_plugin_v1(void);
 *
 * agate never loads the DLL into its own process: it spawns
 * `agate.exe --plugin-host` which loads the DLL, calls into it and exits. A
 * crash, C++ exception or hang inside the plugin therefore cannot take the
 * application down.
 */

#ifndef AGATE_PLUGIN_H
#define AGATE_PLUGIN_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define AGATE_PLUGIN_ABI_VERSION 1

/* One input extension and the system output format it converts to. */
typedef struct AgatePluginFormat {
    /* Input extension: lowercase letters/digits, no dot, [a-z0-9]{1,16}. */
    const char* extension;
    /* Output format: one of pdf,png,jpg,jpeg,webp,bmp,tif,tiff. */
    const char* output;
} AgatePluginFormat;

typedef struct AgatePluginV1 {
    /* Must be AGATE_PLUGIN_ABI_VERSION. */
    uint32_t abi_version;
    /* Unique id, [a-z0-9._-]{1,64}. */
    const char* id;
    /* Display name (UTF-8). */
    const char* name;
    /* Version string (UTF-8). */
    const char* version;
    /* Number of entries in `formats` (1..64). */
    uint32_t format_count;
    const AgatePluginFormat* formats;

    /*
     * Convert `src` (a UTF-8 file path) into `dst` (a path in a writable temp
     * directory). Return 0 on success; any other value is an error and the
     * message must be written into `err` as a NUL-terminated UTF-8 string.
     *
     * Do NOT let a C++ exception or a panic cross this boundary (undefined
     * behaviour). The function may be called from a worker thread that is not
     * the one that loaded the DLL.
     */
    int32_t (*convert)(const char* src, const char* dst, char* err, size_t err_cap);

    /* Optional (may be NULL). */ 
    void (*shutdown)(void);
} AgatePluginV1;

__declspec(dllexport) const AgatePluginV1* agate_plugin_v1(void);

#ifdef __cplusplus
}
#endif

#endif /* AGATE_PLUGIN_H */
