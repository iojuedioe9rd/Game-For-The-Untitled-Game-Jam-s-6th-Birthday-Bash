#pragma once
#include "Engine/Core/Base.h"
#include "VBO.h"

namespace Engine
{
	class ENGINE_API VAO
	{
	public:
		static Ref<VAO> Create();

		bool isDeleted = false;
		uint32_t ID;

		VAO();
		~VAO();

		void LinkVBO(Ref<VBO> VBO, uint32_t layout, uint32_t numComponents, uint32_t type, size_t stride, void* offset);
		void Bind() const;
		void Unbind();
		void Delete();

	};
}