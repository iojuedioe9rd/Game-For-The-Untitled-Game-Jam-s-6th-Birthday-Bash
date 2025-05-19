#pragma once
#include "Engine/Core/Base.h"
#include "Engine/Core/Timestep.h"
#include "Engine/Core/Window.h"
#include <Engine/Renderer/Shader.h>
#include "Engine/Renderer/EBO.h"
#include "Engine/Renderer/VAO.h"
#include "Engine/Renderer/Texture.h"
#include "Engine/Core/Camera.h"
#include "ecs/ecs.h"

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
		
		Ref<Shader> m_Shader;
		Ref<VAO> VAO1;
		Ref<VBO> VBO1;
		Ref<EBO> EBO1;
		Ref<Texture> m_Texture;

		ecs::Manager m_Manager;
		ecs::Entity m_Player;

		Camera m_Camera;

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
