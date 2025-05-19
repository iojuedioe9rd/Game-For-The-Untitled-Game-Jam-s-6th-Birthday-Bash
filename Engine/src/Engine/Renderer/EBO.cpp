#include "EBO.h"
#include "Platform/OpenGL/gl.h"

namespace Engine
{
	Ref<EBO> EBO::Create(uint32_t* indices, size_t size)
	{
		return CreateRef<EBO>(indices, size);
	}

	EBO::EBO(uint32_t* indices, size_t size)
	{
		glGenBuffers(1, &ID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);
	}

	EBO::~EBO()
	{
		Delete();
	}

	void EBO::Bind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
	}

	void EBO::Unbind()
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	void EBO::Delete()
	{
		if (isDeleted)
		{
			return;
		}
		isDeleted = true;

		glDeleteBuffers(1, &ID);
		
	}
}