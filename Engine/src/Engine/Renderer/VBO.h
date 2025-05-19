#pragma once
#include "Engine/Core/Base.h"


namespace Engine
{
	class ENGINE_API VBO
	{
	public:
		static Ref<VBO> Create(float* vertices, size_t size);

		bool isDeleted = false;
		uint32_t ID;
		
		VBO(float* vertices, size_t size);
		~VBO();

		void Bind() const;
		void Unbind();
		void Delete();

	};
}