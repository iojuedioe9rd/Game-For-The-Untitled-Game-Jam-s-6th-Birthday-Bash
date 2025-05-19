#include "VBO.h"
#include "Platform/OpenGL/gl.h"

namespace Engine
{
	Ref<VBO> VBO::Create(float* vertices, size_t size)
	{
		return CreateRef<VBO>(vertices, size);
	}

	VBO::VBO(float* vertices, size_t size)
	{
		glGenBuffers(1, &ID);
		glBindBuffer(GL_ARRAY_BUFFER, ID);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
	}

	VBO::~VBO()
	{
		Delete();
	}

	void VBO::Bind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, ID);
	}

	void VBO::Unbind()
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void VBO::Delete()
	{
		if (isDeleted)
		{
			return;
		}
		isDeleted = true;

		glDeleteBuffers(1, &ID);
		
	}
}