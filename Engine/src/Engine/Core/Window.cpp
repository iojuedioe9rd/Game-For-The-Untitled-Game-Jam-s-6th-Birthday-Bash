#include "Window.h"
#ifdef ENGINE_PLATFORM_WINDOWS
#include "Platform/Windows/WindowsWindow.h"
#elif defined(ENGINE_PLATFORM_WEBGL)
#include "Platform/WebGL/WebGLWindow.h"
#endif // ENGINE_PLATFORM_WINDOWS

#include "Platform/OpenGL/gl.h"

namespace Engine
{
	Ref<Window> Window::New(const WindowProps& props)
	{
#ifdef ENGINE_PLATFORM_WINDOWS
		return CreateRef<WindowsWindow>(props);
#elif defined(ENGINE_PLATFORM_WEBGL)
		return CreateRef<WebGLWindow>(props);
#endif // ENGINE_PLATFORM_WINDOWS
	}
	
	Window::Window(const WindowProps& props)
	{
	}

	Window::~Window()
	{
	}

	void Window::ResizeGL(uint32_t w, uint32_t h)
	{
		glViewport(0, 0, w, h);
	}
}