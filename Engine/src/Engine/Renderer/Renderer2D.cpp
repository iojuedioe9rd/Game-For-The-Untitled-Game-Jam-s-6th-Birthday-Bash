// Renderer2D.cpp
#include "Renderer2D.h"
#include <array>
#include "Engine/Math/Math.h"
#include "Platform/OpenGL/gl.h"
#include <glm/gtc/type_ptr.hpp>

namespace Engine {

    

    struct Renderer2DData {
        uint32_t quadVAO = 0, quadVBO = 0, quadIBO = 0;
        Ref<Shader> shader;
        GLint u_ViewProjLoc = -1;

		Ref<Texture> whiteTexture;

        glm::mat4 viewProj;
    };

    static Renderer2DData* s_Data = nullptr;

    void Renderer2D::Init()
    {
        s_Data = new Renderer2DData();

        // 1) Shader
        s_Data->shader = Shader::Create("assets/shaders/2d_vs.glsl", "assets/shaders/2d_fs.glsl");
        s_Data->u_ViewProjLoc = glGetUniformLocation(s_Data->shader->ID, "u_ViewProjection");

        // 2) White 1x1 texture at slot 0
        uint32_t white = 0xffffffff;
        s_Data->whiteTexture = Texture::Create(1, 1, (void*)&white);

        float vertex[] =
        {
            -0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
             0.5f, -0.5f, 0.0f, 1.0f, 1.0f,
             0.5f,  0.5f, 0.0f, 1.0f, 0.0f,
            -0.5f,  0.5f, 0.0f, 0.0f, 0.0f
        };

		int indices[] =
		{
			0, 1, 2,
			2, 3, 0
		};

		// 3) Quad VAO
		glGenVertexArrays(1, &s_Data->quadVAO);
		glBindVertexArray(s_Data->quadVAO);
		// 4) Quad VBO
		glGenBuffers(1, &s_Data->quadVBO);
		glBindBuffer(GL_ARRAY_BUFFER, s_Data->quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertex), vertex, GL_STATIC_DRAW);
		// 5) Quad IBO
		glGenBuffers(1, &s_Data->quadIBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_Data->quadIBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
		// 6) Vertex attributes
        // Position: 3 floats, starting at offset 0
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // TexCoord: 2 floats, starting after 3 floats (i.e. offset = 3 * sizeof(float))
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

		// Unbind VAO
		glBindVertexArray(0);

		// Unbind VBO
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// Unbind IBO
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        
        
    }

    void Renderer2D::Shutdown()
    {
        glDeleteBuffers(1, &s_Data->quadVBO);
        glDeleteBuffers(1, &s_Data->quadIBO);
        glDeleteVertexArrays(1, &s_Data->quadVAO);
        delete s_Data;
        s_Data = nullptr;
    }

    void Renderer2D::BeginScene(const glm::mat4& viewProj)
    {
        s_Data->viewProj = viewProj;
    }

    void Renderer2D::DrawQuad(const glm::mat4& transform, const glm::vec4& color)
    {
        DrawQuad(transform, s_Data->whiteTexture, color);
    }

    void Renderer2D::DrawQuad(const glm::mat4& transform, const Ref<Texture>& texture, const glm::vec4& color)
    {
		s_Data->shader->Activate();
		glUniformMatrix4fv(s_Data->u_ViewProjLoc, 1, GL_FALSE, glm::value_ptr(s_Data->viewProj));
		glUniform4f(glGetUniformLocation(s_Data->shader->ID, "u_Color"), color.r, color.g, color.b, color.a);
		glUniform1i(glGetUniformLocation(s_Data->shader->ID, "u_Texture"), 0);
		texture->Bind();
		glBindVertexArray(s_Data->quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, s_Data->quadVBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_Data->quadIBO);
		glm::mat4 mvp = s_Data->viewProj * transform;
		glUniformMatrix4fv(glGetUniformLocation(s_Data->shader->ID, "u_MVP"), 1, GL_FALSE, glm::value_ptr(mvp));
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
		glBindVertexArray(0);
    }

    void Renderer2D::EndScene()
    {
        
    }

} // namespace Engine
