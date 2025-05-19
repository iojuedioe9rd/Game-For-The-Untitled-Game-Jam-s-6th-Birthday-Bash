#ifndef FELINE_LOAD_H
#define FELINE_LOAD_H

#ifndef __cplusplus
#define bool char
#endif // !__cplusplus


#ifdef __cplusplus
extern "C" {
#endif

#include "Engine/Core/PlatformDetection.h"
#include <stdint.h>

typedef struct file {
    bool           is_null_terminated;
    uint32_t       crc32;            // ← checksum of buffer
    const char*    filename;
    unsigned char* buffer;
    size_t         size;
    
    
} File;

typedef File (*CustomFileLoader)(const char* filename, bool null_terminated);

ENGINE_API void      feline_load_init();
ENGINE_API void      feline_load_shutdown();
ENGINE_API void      feline_load_clear();

ENGINE_API File      feline_load_file(const char* filename, bool null_terminated);
ENGINE_API File      feline_load_reload_file(const char* filename, bool null_terminated);
ENGINE_API void      feline_load_unload_file(const char* filename);

ENGINE_API void      feline_load_add_custom_loader(CustomFileLoader loader);

#ifdef __cplusplus
}
#endif

#endif /* FELINE_LOAD_H */
