#pragma once
#include "Engine/Core/Base.h"


namespace Engine
{
	class ENGINE_API EBO
	{
	public:
		static Ref<EBO> Create(uint32_t* indices, size_t size);

		bool isDeleted = false;
		uint32_t ID;
		
		EBO(uint32_t* indices, size_t size);
		~EBO();

		void Bind() const;
		void Unbind();
		void Delete();

	};
}