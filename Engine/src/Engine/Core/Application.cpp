#include "Application.h"
#include <SDL3/SDL.h>
#include "Platform/OpenGL/gl.h"
#include "Input.h"


namespace Engine
{
	Application* Application::s_Instance = nullptr;



	

	Application::Application()
	{
		Input::Init();
		s_Instance = this;
		m_Running = true;
		m_Window = Window::New();
	}

	Application::~Application() 
	{
	}

#ifdef ENGINE_PLATFORM_WEBGL
	bool Application::WEB_GL_Update(double time, void* userData)
	{
		auto* app = static_cast<Application*>(userData);
		app->Update();
		return true;
	}
#endif

	void Application::Run()
	{
		m_Window->Show();
	}

	static float timer = 0.0f;
	void Application::Update()
	{
		float time = static_cast<float>(SDL_GetTicks()) / 1000.0f;
		float dt = time - lastTS;
		lastTS = time;
		timer += dt;

		float red = 0.5f + 0.5f * SDL_sin(timer);
		float green = 0.5f + 0.5f * SDL_sin(timer + SDL_PI_D * 2.0 / 3.0);
		float blue = 0.5f + 0.5f * SDL_sin(timer + SDL_PI_D * 4.0 / 3.0);

		glClearColor(red, green, blue, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		m_Window->OnUpdate();

		m_fistFime = false;
	}

	void Application::ResizeGL(uint32_t w, uint32_t h)
	{
		m_Window->ResizeGL(w, h);
	}
}
