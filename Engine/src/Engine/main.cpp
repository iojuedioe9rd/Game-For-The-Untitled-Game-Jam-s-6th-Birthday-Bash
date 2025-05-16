#include "main.h"
#include <cstdlib>
#include <cstdio>

#if defined(ENGINE_PLATFORM_WEBGL)
#include <emscripten.h>
#include <emscripten/html5.h>
EM_JS(void, EM_DEBUGBREAK, (const char* file, int line), {
		console.error("Debug break at " + UTF8ToString(file) + ":" + line + "!");
		debugger;
});
#endif

ENGINE_API void Test()
{
	printf("Hello, World from the engine!\n");
}
