#pragma once
#include "Engine/Core/Base.h"
#include <glm/glm.hpp>


namespace Engine::Math {

	bool ENGINE_API DecomposeTransform(const glm::mat4& transform, glm::vec3& outTranslation, glm::vec3& outRotation, glm::vec3& outScale);

	glm::mat4 ENGINE_API ComposeTransform(const glm::vec3 pos, const glm::vec3 scale, const glm::vec3 rotation);

}