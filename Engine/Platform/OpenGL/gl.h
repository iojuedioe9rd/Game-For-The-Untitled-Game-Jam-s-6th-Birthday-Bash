#pragma once
#include "Engine/Core/PlatformDetection.h"

#ifdef ENGINE_PLATFORM_WEBGL
#include <GLES3/gl3.h>
#else
#include "glad/glad.h"
#endif // ENGINE_PLATFORM_WEBGL

