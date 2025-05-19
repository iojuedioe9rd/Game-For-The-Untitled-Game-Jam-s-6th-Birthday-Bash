#include "LinuxWindow.h"
#include "glad/glad.h"
#include <SDL3/SDL.h>

namespace Engine
{
	static bool s_SDLInitialized = false;

	LinuxWindow::LinuxWindow(const WindowProps& props)
		: Window(props)
	{
		Init(props);
	}

	LinuxWindow::~LinuxWindow()
	{
		Shutdown();
	}

	void LinuxWindow::Show()
	{
		SDL_ShowWindow(m_Data.Window);
	}

	void LinuxWindow::Hide()
	{
		SDL_HideWindow(m_Data.Window);
	}

	int LinuxWindow::GetWidth()
	{
		return m_Data.Width;
	}

	int LinuxWindow::GetHeight()
	{
		return m_Data.Height;
	}

	void* LinuxWindow::GetRaw()
	{
		return &m_Data;
	}

	void LinuxWindow::SetVSync(bool enabled)
	{
		m_Data.VSync = enabled;
	}

	bool LinuxWindow::IsVSync() const
	{
		return m_Data.VSync;
	}

	void LinuxWindow::OnUpdate()
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

	void LinuxWindow::Init(const WindowProps& props)
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


		glEnable(GL_DEPTH_TEST);
		glViewport(0, 0, m_Data.Width, m_Data.Height);

	}

	void LinuxWindow::Shutdown()
	{
		SDL_DestroyWindow(m_Data.Window);
	}
}