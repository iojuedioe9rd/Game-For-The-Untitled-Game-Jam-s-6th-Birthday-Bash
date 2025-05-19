#pragma once

#include "Engine/Core/Base.h"

namespace Engine
{
	class ENGINE_API Shader
	{
	public:
		uint32_t ID;
		static Ref<Shader> Create(const char* vertexFilePath, const char* colourFilePath);

		Shader(const char* vertexFilePath, const char* colourFilePath);
		~Shader();

		void Activate() const;
		void Delete();
	private:
		void CheckCompileErrors(uint32_t shader, const char* type);
		void CheckLinkErrors(uint32_t program);

	};
}