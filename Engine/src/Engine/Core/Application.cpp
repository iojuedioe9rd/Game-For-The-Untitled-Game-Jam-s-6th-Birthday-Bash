#include "Application.h"
#include <SDL3/SDL.h>
#include "Platform/OpenGL/gl.h"
#include "feline_load.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Input.h"
#include "Engine/Renderer/Renderer2D.h"
#include "Engine/Math/Math.h"

// Vertex Shader source code
// Vertices coordinates
GLfloat vertices[] =
{ //     COORDINATES     /        COLORS          /    TexCoord   /        NORMALS       //
	-0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f, 	 0.0f, 0.0f,      0.0f, -1.0f, 0.0f, // Bottom side
	-0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	 0.0f, 5.0f,      0.0f, -1.0f, 0.0f, // Bottom side
	 0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	 5.0f, 5.0f,      0.0f, -1.0f, 0.0f, // Bottom side
	 0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,	 5.0f, 0.0f,      0.0f, -1.0f, 0.0f, // Bottom side

	-0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f, 	 0.0f, 0.0f,     -0.8f, 0.5f,  0.0f, // Left Side
	-0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	 5.0f, 0.0f,     -0.8f, 0.5f,  0.0f, // Left Side
	 0.0f, 0.8f,  0.0f,     0.92f, 0.86f, 0.76f,	 2.5f, 5.0f,     -0.8f, 0.5f,  0.0f, // Left Side

	-0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	 5.0f, 0.0f,      0.0f, 0.5f, -0.8f, // Non-facing side
	 0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	 0.0f, 0.0f,      0.0f, 0.5f, -0.8f, // Non-facing side
	 0.0f, 0.8f,  0.0f,     0.92f, 0.86f, 0.76f,	 2.5f, 5.0f,      0.0f, 0.5f, -0.8f, // Non-facing side

	 0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	 0.0f, 0.0f,      0.8f, 0.5f,  0.0f, // Right side
	 0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,	 5.0f, 0.0f,      0.8f, 0.5f,  0.0f, // Right side
	 0.0f, 0.8f,  0.0f,     0.92f, 0.86f, 0.76f,	 2.5f, 5.0f,      0.8f, 0.5f,  0.0f, // Right side

	 0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,	 5.0f, 0.0f,      0.0f, 0.5f,  0.8f, // Facing side
	-0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f, 	 0.0f, 0.0f,      0.0f, 0.5f,  0.8f, // Facing side
	 0.0f, 0.8f,  0.0f,     0.92f, 0.86f, 0.76f,	 2.5f, 5.0f,      0.0f, 0.5f,  0.8f  // Facing side
};

// Indices for vertices order
GLuint indices[] =
{
	0, 1, 2, // Bottom side
	0, 2, 3, // Bottom side
	4, 6, 5, // Left side
	7, 9, 8, // Non-facing side
	10, 12, 11, // Right side
	13, 15, 14 // Facing side
};

namespace Engine
{
	Application* Application::s_Instance = nullptr;

	GLuint framebuffer;
	GLuint texture;
	GLuint renderbuffer;
	GLuint bloomTexture;

	struct TransformComponent
	{
		glm::vec3 Position;
		glm::vec3 Rotation;
		glm::vec3 Scale;
	};

	struct Renderer
	{
		glm::vec4 colour;
	};

	Application::Application()
		: m_Camera(19, 18, glm::vec3(0.0f, 0.0f, 2.0f))
	{
		Input::Init();
		feline_load_init();
		s_Instance = this;
		m_Running = true;
		m_Window = Window::New();

		m_Manager = ecs::Manager();
		m_Player = m_Manager.CreateEntity();
		m_Manager.Refresh();

		m_Player.Add<TransformComponent>(glm::vec3(0), glm::vec3(0), glm::vec3(1));
		m_Player.Add<Renderer>(glm::vec4(1, 0, 0, 1));
		m_Manager.Refresh();
	}

	Application::~Application() 
	{
		Renderer2D::Shutdown();
		feline_load_shutdown();
	}

#ifdef ENGINE_PLATFORM_WEBGL
	bool Application::WEB_GL_Update(double time, void* userData)
	{
		auto* app = static_cast<Application*>(userData);
		app->Update();
		return true;
	}
#endif

	void Application::Run()
	{
		Renderer2D::Init();
		m_Shader = Shader::Create("assets/shaders/shader_vs.glsl", "assets/shaders/shader_fs.glsl");
		m_Texture = Texture::Create("assets/textures/Player.png", GL_TEXTURE_2D, GL_TEXTURE0);
		m_Texture->texUnit(m_Shader, "tex0", 0);
		m_Camera.Resize(m_Window->GetWidth(), m_Window->GetHeight());
		m_Camera.Position.z = 10.f;

		// Generates Vertex Array Object and binds it
		VAO1 = VAO::Create();
		VAO1->Bind();

		// Generates Vertex Buffer Object and links it to vertices
		VBO1 = VBO::Create(vertices, sizeof(vertices));
		// Generates Element Buffer Object and links it to indices
		EBO1 = EBO::Create(indices, sizeof(indices));

		// Links VBO to VAO
		VAO1->LinkVBO(VBO1, 0, 3, GL_FLOAT, 11 * sizeof(float), (void*)0);
		VAO1->LinkVBO(VBO1, 1, 3, GL_FLOAT, 11 * sizeof(float), (void*)(3 * sizeof(float)));
		VAO1->LinkVBO(VBO1, 2, 2, GL_FLOAT, 11 * sizeof(float), (void*)(6 * sizeof(float)));
		VAO1->LinkVBO(VBO1, 3, 3, GL_FLOAT, 11 * sizeof(float), (void*)(8 * sizeof(float)));

		// Unbind all to prevent accidentally modifying them
		VAO1->Unbind();
		VBO1->Unbind();
		EBO1->Unbind();
		
		// Gets ID of uniform called "scale"
		
		m_Window->Show();

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	static float timer = 0.0f;

	static float rotation = 0.0f;
	void Application::Update()
	{
		float time = static_cast<float>(SDL_GetTicks()) / 1000.0f;
		float dt = time - lastTS;
		lastTS = time;
		timer += dt;

		float red = 0.5f + 0.5f * SDL_sin(timer);
		float green = 0.5f + 0.5f * SDL_sin(timer + SDL_PI_D * 2.0 / 3.0);
		float blue = 0.5f + 0.5f * SDL_sin(timer + SDL_PI_D * 4.0 / 3.0);

		glClearColor(red, green, blue, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (Input::IsPressed('w'))
		{
			m_Camera.Position.y += 5 * dt;
		}

		if (Input::IsPressed('s'))
		{
			m_Camera.Position.y -= 5 * dt;
		}

		if (Input::IsPressed('a'))
		{
			m_Camera.Position.x -= 5 * dt;
		}

		if (Input::IsPressed('d'))
		{
			m_Camera.Position.x += 5 * dt;
		}

		rotation += dt * 21;


		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 proj = glm::mat4(1.0f);

		glm::vec3 lightPos = glm::vec3(0.5f, 0.5f, 0.5f);
		glm::vec4 lightColor = glm::vec4(1.0, 1.0f, 1.0f, 1.0f);

		// Assigns different transformations to each matrix
		model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 1.0f, 0.0f));
		//model = glm::scale(model, glm::vec3(2,2,2));

		view = glm::translate(view, glm::vec3(0.0f, -0.5f, -2.0f));
		proj = glm::perspective(glm::radians(45.0f), (float)m_Window->GetWidth() / m_Window->GetHeight(), 0.1f, 100.0f);

		// Use shader and draw
		m_Shader->Activate();

		// Set light and camera uniforms
		glUniform4f(glGetUniformLocation(m_Shader->ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
		glUniform3f(glGetUniformLocation(m_Shader->ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
		glUniform3f(glGetUniformLocation(m_Shader->ID, "camPos"), m_Camera.Position.x, m_Camera.Position.y, m_Camera.Position.z);
		m_Camera.Matrix(90, 0.01f, 100.0f, m_Shader, "camMatrix");

		// Outputs the matrices into the Vertex Shader
		int modelLoc = glGetUniformLocation(m_Shader->ID, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		// Bind texture and set uniform for scale
		m_Texture->Bind();
		GLuint uniID = glGetUniformLocation(m_Shader->ID, "scale");
		glUniform1f(uniID, 1);

		Renderer2D::BeginScene(m_Camera.Matrix(90, 0.1f, 100.0f));

		for (auto [e, pos, renderer] : m_Manager.EntitiesWith<TransformComponent, Renderer>()) {
			glm::vec3 position = pos.Position;
			glm::vec3 rotation = pos.Rotation;
			glm::vec3 scale = pos.Scale;
			glm::mat4 transform = Math::ComposeTransform(
				position,
				scale,
				rotation
			);
			Renderer2D::DrawQuad(transform, m_Texture, renderer.colour);
		}
		m_Manager.Refresh();

		/*transform = Math::ComposeTransform(
			glm::vec3(0.0f, 2, 0.0f),
			glm::vec3(2.0f, 2.0f, 1.0f),
			glm::vec3(0.0f, 0.0f, rotation)
		);

		Renderer2D::DrawQuad(transform, m_Texture, glm::vec4(1.0f));*/

		Renderer2D::EndScene();

		m_Window->OnUpdate();

		m_fistFime = false;
	}

	void Application::ResizeGL(uint32_t w, uint32_t h)
	{
		m_Window->ResizeGL(w, h);
	}
}
