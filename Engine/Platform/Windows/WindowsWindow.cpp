#include "WindowsWindow.h"
#include "glad/glad.h"
#include <SDL3/SDL.h>
#include <iostream>

namespace Engine
{
	static bool s_SDLInitialized = false;

	WindowsWindow::WindowsWindow(const WindowProps& props)
		: Window(props)
	{
		Init(props);
	}

	WindowsWindow::~WindowsWindow()
	{
		Shutdown();
	}

	void WindowsWindow::Show()
	{
		SDL_ShowWindow(m_Data.Window);
	}

	void WindowsWindow::Hide()
	{
		SDL_HideWindow(m_Data.Window);
	}

	int WindowsWindow::GetWidth()
	{
		return m_Data.Width;
	}

	int WindowsWindow::GetHeight()
	{
		return m_Data.Height;
	}

	void* WindowsWindow::GetRaw()
	{
		return &m_Data;
	}

	void WindowsWindow::SetVSync(bool enabled)
	{
		m_Data.VSync = enabled;
	}

	bool WindowsWindow::IsVSync() const
	{
		return m_Data.VSync;
	}

	void WindowsWindow::OnUpdate()
	{
		int w = -1;
		int h = -1;
		SDL_GetWindowSize(m_Data.Window, &w, &h);

		if (w == -1 || h == -1)
		{
			return;
		}

		if (w != m_Data.Width || h != m_Data.Height)
		{
			m_Data.Width = w;
			m_Data.Height = h;

			ResizeGL(w, h);
		}

		SDL_GL_SwapWindow(m_Data.Window);
	}

	void APIENTRY GLDebugOutput(
		GLenum source,
		GLenum type,
		GLuint id,
		GLenum severity,
		GLsizei length,
		const GLchar* message,
		const void* userParam)
	{

		std::cerr << "GL DEBUG: " << message << std::endl;

		if (severity == GL_DEBUG_SEVERITY_HIGH)
			std::cerr << "SEVERITY: HIGH" << std::endl;

		// You can also exit or throw here on critical errors
	}


	void WindowsWindow::Init(const WindowProps& props)
	{
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
		
		
		m_Data.Title = props.Title;
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;

		if (!s_SDLInitialized)
		{
			SDL_Init(SDL_INIT_VIDEO);
		}

		m_Data.Window = SDL_CreateWindow(m_Data.Title.c_str(), m_Data.Width, m_Data.Height, SDL_WINDOW_HIDDEN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
		
		SDL_GL_CreateContext(m_Data.Window);
		if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
		{

		}
		glDebugMessageCallback(GLDebugOutput, nullptr);
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); // optional: makes messages synchronous
		glEnable(GL_DEPTH_TEST);
		glViewport(0, 0, m_Data.Width, m_Data.Height);

	}

	void WindowsWindow::Shutdown()
	{
		SDL_DestroyWindow(m_Data.Window);
	}
}