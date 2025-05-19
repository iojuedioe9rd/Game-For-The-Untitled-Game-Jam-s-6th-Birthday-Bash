#pragma once

#include "Engine/Core/Base.h"
#include <glm/glm.hpp>
#include "Engine/Renderer/Shader.h"

namespace Engine
{
	class ENGINE_API Camera
	{
	public:
		glm::vec3 Position;
		glm::vec3 Orientation = glm::vec3(0, 0, -1);
		glm::vec3 Up = glm::vec3(0, 1, 0);

		// Stores the width and height of the window
		uint32_t width;
		uint32_t height;


		// Camera constructor to set up initial values
		Camera(int width, int height, glm::vec3 position);

		// Updates and exports the camera matrix to the Vertex Shader
		void Matrix(float FOVdeg, float nearPlane, float farPlane, Ref<Shader> shader, const char* uniform) const;
		glm::mat4 Matrix(float FOVdeg, float nearPlane, float farPlane) const;

		void Resize(uint32_t width, uint32_t height)
		{
			this->width = width;
			this->height = height;
		}
	};
}