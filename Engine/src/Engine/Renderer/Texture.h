#pragma once

#include "Engine/Core/Base.h"
#include "Shader.h"

namespace Engine
{

	class ENGINE_API Texture
	{
	public:
		uint32_t ID;
		uint32_t type;
		
		static Ref<Texture> Create(int width, int height, void* data);
		static Ref<Texture> Create(const char* filename, uint32_t texType, uint32_t slot);
		Texture(int width, int height, void* data);
		Texture(const char* filename, uint32_t texType, uint32_t slot);
		~Texture();

		// Assigns a texture unit to a texture
		void texUnit(Ref<Shader> shader, const char* uniform, uint32_t unit);

		// Binds a texture
		void Bind() const;

		// Unbinds a texture
		void Unbind() const;
	};

}