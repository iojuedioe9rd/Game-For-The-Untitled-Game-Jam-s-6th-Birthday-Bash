#include "VAO.h"
#include "Platform/OpenGL/gl.h"


namespace Engine
{
	Ref<VAO> VAO::Create()
	{
		return CreateRef<VAO>();
	}

	VAO::VAO()
	{
		glGenVertexArrays(1, &ID);
	}

	VAO::~VAO()
	{
		Delete();
	}

	void VAO::LinkVBO(Ref<VBO> VBO, uint32_t layout, uint32_t numComponents, uint32_t type, size_t stride, void* offset)
	{
		VBO->Bind();
		glVertexAttribPointer(layout, numComponents, type, GL_FALSE, stride, offset);
		glEnableVertexAttribArray(layout);
		VBO->Unbind();
	}

	void VAO::Bind() const
	{
		glBindVertexArray(ID);
	}

	void VAO::Unbind()
	{
		glBindVertexArray(0);
	}

	void VAO::Delete()
	{
		if (isDeleted)
		{
			return;
		}

		isDeleted = true;
		
		glDeleteVertexArrays(1, &ID);
	}

}

