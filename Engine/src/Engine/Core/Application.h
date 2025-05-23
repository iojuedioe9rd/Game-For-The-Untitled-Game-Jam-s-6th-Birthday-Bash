#pragma once
#include "Engine/Core/Base.h"
#include "Engine/Core/Timestep.h"
#include "Engine/Core/Window.h"
#include <Engine/Renderer/Shader.h>
#include "Engine/Renderer/EBO.h"
#include "Engine/Renderer/VAO.h"
#include "Engine/Renderer/Texture.h"
#include "Engine/Core/Camera.h"
#include <box2d/b2_world.h>
#include "ecs/ecs.h"
#include <typeinfo>
#include <utility>
#include "Engine/Core/Audio.h"
#include "Components.h"

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
		static Window& GetWindow() { return *s_Instance->m_Window; }

		void ResizeGL(uint32_t w, uint32_t h);

		ecs::Entity NewEntity();

		template<typename T, typename... Args>
		T& NewCommponent(ecs::Entity entity, Args&&... args)
		{
			T& component = entity.Add<T>(std::forward<Args>(args)...);

			InitCommponent(entity, &component, typeid(T).name());

			return component;
		}

		

		void AddUpdateFunc(std::function<void(float, ecs::Manager&)> func)
		{
			m_UpdateFuncList.push_back(func);
		}

		ecs::Manager& GetManager()
		{
			return m_Manager;
		}

		Camera Camera;

		std::vector<Components::Rigidbody2DComponent> m_RigidBodiesToRemove;

	private:
		
		void InitCommponent(ecs::Entity entity, void* commp, const std::string type_name);

		

		Ref<Audio> m_Audio;

		b2World* m_World;

		Ref<Shader> m_Shader;
		Ref<VAO> VAO1;
		Ref<VBO> VBO1;
		Ref<EBO> EBO1;

		ecs::Manager m_Manager;
		ecs::Entity m_Player;
		std::vector<std::function<void(float, ecs::Manager&)>> m_UpdateFuncList;

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
