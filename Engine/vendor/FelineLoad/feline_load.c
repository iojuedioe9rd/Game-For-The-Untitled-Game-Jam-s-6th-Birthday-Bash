#include "feline_load.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <nmmintrin.h>   // for _mm_crc32_u64, _mm_crc32_u8

typedef struct feline_loader {
    File*               files;
    size_t               file_count;
    size_t               file_capacity;
    CustomFileLoader*   custom_loaders;
    size_t               loader_count;
    size_t               loader_capacity;
} FelineLoader;

static FelineLoader* g_feline_loader = NULL;

// SSE4.2 CRC32 helper
#if defined(__SSE4_2__) && !defined(__EMSCRIPTEN__)
#include <nmmintrin.h>  // For _mm_crc32_u64 and _mm_crc32_u8

static uint32_t crc32_sse42(const unsigned char *data, size_t len) {
    uint64_t crc = 0xFFFFFFFFu;
    size_t   i   = 0;

    // process 8-byte chunks
    for (; i + 8 <= len; i += 8) {
        uint64_t chunk;
        memcpy(&chunk, data + i, 8);
        crc = _mm_crc32_u64(crc, chunk);
    }
    // process remaining bytes
    for (; i < len; ++i) {
        crc = _mm_crc32_u8(crc, data[i]);
    }
    return (uint32_t)~crc;
}

#else

// Portable fallback (bitwise CRC32 implementation)
static uint32_t crc32_sse42(const unsigned char *data, size_t len) {
    uint32_t crc = ~0u;
    while (len--) {
        crc ^= *data++;
        for (int i = 0; i < 8; i++)
            crc = (crc >> 1) ^ (0xEDB88320 & -(crc & 1));
    }
    return ~crc;
}

#endif

void feline_load_init() {
    g_feline_loader = malloc(sizeof(FelineLoader));
    g_feline_loader->file_count    = 0;
    g_feline_loader->file_capacity = 4;
    g_feline_loader->files         = malloc(sizeof(File) * g_feline_loader->file_capacity);

    g_feline_loader->loader_count    = 0;
    g_feline_loader->loader_capacity = 4;
    g_feline_loader->custom_loaders  = malloc(sizeof(CustomFileLoader) * g_feline_loader->loader_capacity);
}

void feline_load_shutdown() {
    if (!g_feline_loader) return;
    feline_load_clear();
    free(g_feline_loader->files);
    free(g_feline_loader->custom_loaders);
    free(g_feline_loader);
    g_feline_loader = NULL;
}

void feline_load_clear() {
    if (!g_feline_loader) return;
    for (size_t i = 0; i < g_feline_loader->file_count; i++) {
        free((void*)g_feline_loader->files[i].filename);
        free(g_feline_loader->files[i].buffer);
    }
    g_feline_loader->file_count = 0;
}

void feline_load_add_custom_loader(CustomFileLoader loader) {
    if (!g_feline_loader || !loader) return;
    if (g_feline_loader->loader_count == g_feline_loader->loader_capacity) {
        g_feline_loader->loader_capacity *= 2;
        g_feline_loader->custom_loaders = realloc(
            g_feline_loader->custom_loaders,
            sizeof(CustomFileLoader) * g_feline_loader->loader_capacity
        );
    }
    g_feline_loader->custom_loaders[g_feline_loader->loader_count++] = loader;
}

static File load_file_from_disk(const char* filename, bool null_terminated) {
    File empty = {0};
    FILE* f = fopen(filename, "rb");
    if (!f) {
        fprintf(stderr, "Failed to open file: %s\n", filename);
        return empty;
    }

    fseek(f, 0, SEEK_END);
    size_t size = ftell(f);
    rewind(f);

    unsigned char* buffer = null_terminated
        ? malloc(size + 1)
        : malloc(size);
    if (!buffer) {
        fclose(f);
        fprintf(stderr, "Out of memory: %s\n", filename);
        return empty;
    }

    if (fread(buffer, 1, size, f) != size) {
        fclose(f);
        free(buffer);
        fprintf(stderr, "Failed to read file completely: %s\n", filename);
        return empty;
    }
    fclose(f);

    if (null_terminated) buffer[size] = '\0';

    // compute CRC32 on raw data
    uint32_t crc = crc32_sse42((unsigned char*)buffer, size);

    char* name_copy = strdup(filename);
    if (!name_copy) {
        free(buffer);
        fprintf(stderr, "Out of memory (filename copy): %s\n", filename);
        return empty;
    }

    File file = {
        .filename           = name_copy,
        .buffer             = buffer,
        .size               = size,
        .is_null_terminated = null_terminated,
        .crc32              = crc
    };
    return file;
}

File feline_load_file(const char* filename, bool null_terminated) {
    File empty = {0};
    if (!g_feline_loader || !filename) return empty;

    printf("Loading file: %s\n", filename);

    // cached?
    for (size_t i = 0; i < g_feline_loader->file_count; i++) {
        File* f = &g_feline_loader->files[i];

        printf("File already loaded: %s\n", filename);
        printf("File size: %zu bytes\n", f->size); // Print file size
        //printf("File content: %s\n", f->buffer); // Print file content
        if (strcmp(f->filename, filename) == 0) return *f;
    }

    // custom loaders
    for (size_t i = 0; i < g_feline_loader->loader_count; i++) {
        CustomFileLoader loader = g_feline_loader->custom_loaders[i];
        File f = loader(filename, null_terminated);
        if (f.buffer) {
            if (g_feline_loader->file_count == g_feline_loader->file_capacity) {
                g_feline_loader->file_capacity *= 2;
                g_feline_loader->files = realloc(
                    g_feline_loader->files,
                    sizeof(File) * g_feline_loader->file_capacity
                );
            }
            printf("File size: %zu bytes\n", f.size); // Print file size
            printf("File content: %s\n", f.buffer); // Print file content

            g_feline_loader->files[g_feline_loader->file_count++] = f;
            return f;
        }
    }

    

    // disk fallback
    File loaded = load_file_from_disk(filename, null_terminated);
    if (loaded.buffer) {
        if (g_feline_loader->file_count == g_feline_loader->file_capacity) {
            g_feline_loader->file_capacity *= 2;
            g_feline_loader->files = realloc(
                g_feline_loader->files,
                sizeof(File) * g_feline_loader->file_capacity
            );
        }
        g_feline_loader->files[g_feline_loader->file_count++] = loaded;
    }
    printf("File size: %zu bytes\n", loaded.size); // Print file size
    printf("File content: %s\n", loaded.buffer); // Print file content

    return loaded;
}

File feline_load_reload_file(const char* filename, bool null_terminated) {
    feline_load_unload_file(filename);
    return feline_load_file(filename, null_terminated);
}

void feline_load_unload_file(const char* filename) {
    if (!g_feline_loader || !filename) return;

    for (size_t i = 0; i < g_feline_loader->file_count; i++) {
        File* f = &g_feline_loader->files[i];
        if (strcmp(f->filename, filename) == 0) {
            free((void*)f->filename);
            free(f->buffer);
            // shift the rest down
            for (size_t j = i; j + 1 < g_feline_loader->file_count; j++) {
                g_feline_loader->files[j] = g_feline_loader->files[j+1];
            }
            g_feline_loader->file_count--;
            break;
        }
    }
}
