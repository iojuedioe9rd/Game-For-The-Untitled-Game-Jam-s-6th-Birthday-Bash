// Platform detection using predefined macros
#ifdef _WIN32
    /* Windows x64/x86 */
#ifdef USE_DLL
#ifdef DLL_EXPORTS
#define ENGINE_API __declspec(dllexport)
#else
#define ENGINE_API __declspec(dllimport)
#endif
#else
#define ENGINE_API
#endif

#ifdef _WIN64
    /* Windows x64 */
#define ENGINE_PLATFORM_WINDOWS
#else
    /* Windows x86 */
#error "x86 Builds are not supported!"
#endif

#elif defined(__EMSCRIPTEN__)
#include <emscripten.h>
#include <emscripten/html5.h>
#define ENGINE_PLATFORM_WEBGL
#define ENGINE_API

#elif defined(__APPLE__) || defined(__MACH__)
#include <TargetConditionals.h>
#if TARGET_IPHONE_SIMULATOR == 1
#error "IOS simulator is not supported!"
#elif TARGET_OS_IPHONE == 1
#define ENGINE_PLATFORM_IOS
#error "IOS is not supported!"
#elif TARGET_OS_MAC == 1
#define ENGINE_PLATFORM_MACOS
#error "MacOS is not supported!"
#else
#error "Unknown Apple platform!"
#endif

#elif defined(__ANDROID__)
#define ENGINE_PLATFORM_ANDROID
#error "Android is not supported!"

#elif defined(__linux__)
#define ENGINE_PLATFORM_LINUX
#error "Linux is not supported!"

#else
#define ENGINE_API
#error "Unknown platform!"
#endif
