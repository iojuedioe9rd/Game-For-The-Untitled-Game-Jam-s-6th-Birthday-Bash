#pragma once
#include "Engine/Core/Base.h"
#include "Engine/Core/Timestep.h"
#include "Engine/Core/Window.h"


namespace Engine
{
	class ENGINE_API Application 
	{
	public:
		Application();
		virtual ~Application();
		void Run();
		void Update();

		static Application& Get() { return *s_Instance; }

		void ResizeGL(uint32_t w, uint32_t h);

	private:
		

		bool m_fistFime = true;
		bool m_Running = true;
#ifdef ENGINE_PLATFORM_WEBGL
		static bool WEB_GL_Update(double time, void* userData);
#endif // 
		
		Timestep lastTS;
		Ref<Window> m_Window;

		static Application* s_Instance;
	};

	extern Application* CreateApplication();
}
