#include "WebGLWindow.h"
#include "Platform/OpenGL/gl.h"
#include <SDL3/SDL.h>

namespace Engine
{
	static bool s_SDLInitialized = false;

	WebGLWindow::WebGLWindow(const WindowProps& props)
		: Window(props)
	{
		Init(props);
	}

	WebGLWindow::~WebGLWindow()
	{
		Shutdown();
	}

	void WebGLWindow::Show()
	{
		SDL_ShowWindow(m_Data.Window);
	}

	void WebGLWindow::Hide()
	{
		SDL_HideWindow(m_Data.Window);
	}

	int WebGLWindow::GetWidth()
	{
		return m_Data.Width;
	}

	int WebGLWindow::GetHeight()
	{
		return m_Data.Height;
	}

	void* WebGLWindow::GetRaw()
	{
		return &m_Data;
	}

	void WebGLWindow::SetVSync(bool enabled)
	{
		m_Data.VSync = enabled;
	}

	bool WebGLWindow::IsVSync() const
	{
		return m_Data.VSync;
	}

	void WebGLWindow::OnUpdate()
	{
	}

	void WebGLWindow::Init(const WindowProps& props)
	{
		m_Data.Title = props.Title;
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;

		if (!s_SDLInitialized)
		{
			SDL_Init(SDL_INIT_VIDEO);
		}

		m_Data.Window = SDL_CreateWindow(m_Data.Title.c_str(), m_Data.Width, m_Data.Height, SDL_WINDOW_HIDDEN | SDL_WINDOW_RESIZABLE);
#ifdef ENGINE_PLATFORM_WEBGL
		EmscriptenWebGLContextAttributes attrs;
		emscripten_webgl_init_context_attributes(&attrs);
		attrs.alpha = false;
		attrs.majorVersion = 2; // WebGL2 = OpenGL ES 3.0
		EMSCRIPTEN_WEBGL_CONTEXT_HANDLE ctx = emscripten_webgl_create_context("#canvas", &attrs);
		emscripten_webgl_make_context_current(ctx);
#endif // ENGINE_PLATFORM_WEBGL

		glEnable(GL_DEPTH_TEST);
		glViewport(0, 0, m_Data.Width, m_Data.Height);
	}

	void WebGLWindow::Shutdown()
	{
		SDL_DestroyWindow(m_Data.Window);
	}
}