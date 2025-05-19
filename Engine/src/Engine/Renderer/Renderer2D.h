#pragma once
#include "Engine/Core/Base.h"
#include "glm/glm.hpp"
#include "Texture.h"

namespace Engine
{
	class ENGINE_API Renderer2D
	{
	public:
		static void Init();

		static void Shutdown();

		static void BeginScene(const glm::mat4& cam);
		static void DrawQuad(const glm::mat4& transform, const glm::vec4& color);
		static void DrawQuad(const glm::mat4& transform, const Ref<Texture>& texture, const glm::vec4& color = glm::vec4(1.0f));
		//static void DrawQuad(const glm::mat4& transform, const Ref<Texture> texture, const glm::vec4& color);

		static void EndScene();

	private:

	};

}