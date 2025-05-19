#include "Camera.h"
#include "Platform/OpenGL/gl.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Engine
{
	Camera::Camera(int width, int height, glm::vec3 position)
	{
		Camera::width = width;
		Camera::height = height;
		Position = position;
	}

	void Camera::Matrix(float FOVdeg, float nearPlane, float farPlane, Ref<Shader> shader, const char* uniform) const
	{
		// Initializes matrices since otherwise they will be the null matrix
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);

		// Makes camera look in the right direction from the right position
		view = glm::lookAt(Position, Position + Orientation, Up);
		// Adds perspective to the scene
		projection = glm::perspective(glm::radians(FOVdeg), (float)width / height, nearPlane, farPlane);

		// Exports the camera matrix to the Vertex Shader
		glUniformMatrix4fv(glGetUniformLocation(shader->ID, uniform), 1, GL_FALSE, glm::value_ptr(projection * view));
	}

	glm::mat4 Camera::Matrix(float FOVdeg, float nearPlane, float farPlane) const
	{
		// Initializes matrices since otherwise they will be the null matrix
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);

		// Makes camera look in the right direction from the right position
		view = glm::lookAt(Position, Position + Orientation, Up);
		// Adds perspective to the scene
		projection = glm::perspective(glm::radians(FOVdeg), (float)width / height, nearPlane, farPlane);

		return projection * view;
	}
}