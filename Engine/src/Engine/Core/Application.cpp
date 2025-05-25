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
#include "Engine/Core/Components.h"
#include "Engine/Core/Audio.h"

// Box2D
#include "box2d/b2_world.h"
#include "box2d/b2_body.h"
#include "box2d/b2_fixture.h"
#include "box2d/b2_polygon_shape.h"
#include <iostream>

static Engine::Ref<Engine::Texture> s_endingTexture = nullptr;
static Engine::Ref<Engine::Texture> s_skyTexture = nullptr;

float rectangleVertices[] =
{
	// Coords    // texCoords
	 1.0f, -1.0f,  1.0f, 0.0f,
	-1.0f, -1.0f,  0.0f, 0.0f,
	-1.0f,  1.0f,  0.0f, 1.0f,

	 1.0f,  1.0f,  1.0f, 1.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,
	-1.0f,  1.0f,  0.0f, 1.0f
};


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


	Application::Application()
		: Camera(19, 18, glm::vec3(0.0f, 0.0f, 2.0f))
	{
		Input::Init();
		feline_load_init();
		Audio::Init();
		s_Instance = this;
		m_Running = true;
		m_Window = Window::New();


		m_World = new b2World(b2Vec2(0.0f, -9.81));

		m_Manager = ecs::Manager();
		m_Manager.Refresh();

		//m_Player.Add<Components::TransformComponent>(glm::vec3(0), glm::vec3(0), glm::vec3(1));
		//m_Player.Add<Components::Renderer>(glm::vec4(1, 0, 0, 1));
		m_Manager.Refresh();
	}

	static void SetupRB(void* commp, b2World* m_World, ecs::Entity entity)
	{
		Components::Rigidbody2DComponent* rb = static_cast<Components::Rigidbody2DComponent*>(commp);
		b2BodyDef bodyDef;
		bodyDef.type = (b2BodyType)rb->Type;
		bodyDef.position.Set(entity.Get<Components::TransformComponent>().Position.x, entity.Get<Components::TransformComponent>().Position.y);
		bodyDef.angle = glm::radians(entity.Get<Components::TransformComponent>().Rotation.z);
		bodyDef.linearVelocity.Set(0, 0);
		bodyDef.angularVelocity = 0.0f;
		bodyDef.fixedRotation = rb->FixedRotation;
		b2Body* body = m_World->CreateBody(&bodyDef);
		rb->RuntimeBody = body;
	}

	void Application::InitCommponent(ecs::Entity entity, void* commp, const std::string type_name)
	{
		
		std::string Rigidbody2DComponent = typeid(Components::Rigidbody2DComponent).name();
		if (type_name == Rigidbody2DComponent)
		{
			SetupRB(commp, m_World, entity);
		}
		if (type_name == typeid(Components::BoxCollider2DComponent).name())
		{
			
			Components::Rigidbody2DComponent& rb2D = entity.Get<Components::Rigidbody2DComponent>();
			if (rb2D.RuntimeBody.get() == nullptr)
			{
				return;
			}
			Components::BoxCollider2DComponent* box = static_cast<Components::BoxCollider2DComponent*>(commp);
			b2PolygonShape boxShape;
			
			boxShape.SetAsBox(entity.Get<Components::TransformComponent>().Scale.x *box->Size.x, entity.Get<Components::TransformComponent>().Scale.y * box->Size.y);
			b2FixtureDef fixtureDef;
			fixtureDef.shape = &boxShape;
			fixtureDef.density = box->Density;
			fixtureDef.friction = box->Friction;
			fixtureDef.restitution = box->Friction;
			fixtureDef.restitutionThreshold = box->RestitutionThreshold;
			((b2Body*)rb2D.RuntimeBody.get())->CreateFixture(&fixtureDef);
		}
	}

	ecs::Entity Application::NewEntity()
	{
		ecs::Entity entity = m_Manager.CreateEntity();



		return entity;
	}

	Application::~Application() 
	{
		Audio::Shutdown();
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

	Ref<Texture> s_playerTexture = nullptr;

	void Application::Run()
	{
		s_endingTexture = Engine::Texture::Create("assets/Tiled/game_eng.png", GL_TEXTURE_2D, GL_TEXTURE0);
		m_Framebuffer = Framebuffer::Create(m_Window->GetWidth(), m_Window->GetHeight());
		m_FramebufferShader = Shader::Create("assets/shaders/framebuffer_vs.glsl", "assets/shaders/framebuffer_fs.glsl");
		m_endingShader = Shader::Create("assets/shaders/ending_vs.glsl", "assets/shaders/ending_fs.glsl");
		s_skyTexture = Engine::Texture::Create("assets/textures/sky.png", GL_TEXTURE_2D, GL_TEXTURE1);
		s_playerTexture = Engine::Texture::Create("assets/textures/player - Copy.png", GL_TEXTURE_2D, GL_TEXTURE2);

		// Setup full-screen quad VAO/VBO
		glGenVertexArrays(1, &rectVAO);
		glGenBuffers(1, &rectVBO);
		glBindVertexArray(rectVAO);
		glBindBuffer(GL_ARRAY_BUFFER, rectVBO);
		float rectangleVertices[] = {
			// positions // texCoords
			 1.0f, -1.0f, 1.0f, 0.0f,
			-1.0f, -1.0f, 0.0f, 0.0f,
			-1.0f,  1.0f, 0.0f, 1.0f,
			 1.0f,  1.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 1.0f, 0.0f,
			-1.0f,  1.0f, 0.0f, 1.0f
		};
		glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), rectangleVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
		Renderer2D::Init();
		m_Shader = Shader::Create("assets/shaders/shader_vs.glsl", "assets/shaders/shader_fs.glsl");
		//m_Texture = Texture::Create("assets/textures/Player.png", GL_TEXTURE_2D, GL_TEXTURE0);
		//m_Texture->texUnit(m_Shader, "tex0", 0);
		Camera.Resize(m_Window->GetWidth(), m_Window->GetHeight());
		Camera.Position.z = 10.f;

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


		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		m_Audio = Audio::Create("assets/audio/the beginning/auto.wav");

	}

	static float timer = 0.0f;
	static float timer2 = 0.0f;

	static float fWorldX = 1000.4f;
	static float fWorldY = 1000.39f;
	static float fWorldA = 0.77135f;
	static float fNear = 0.026492f;
	static float fFar = 0.199961f;
	static float fFoVHalf = 3.14159f / 4.0f; // 45 degrees in radians

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

		if (isWining)
		{
			float fFarX1 = fWorldX + glm::cos(fWorldA - fFoVHalf) * fFar;
			float fFarY1 = fWorldY + glm::sin(fWorldA - fFoVHalf) * fFar;

			float fNearX1 = fWorldX + glm::cos(fWorldA - fFoVHalf) * fNear;
			float fNearY1 = fWorldY + glm::sin(fWorldA - fFoVHalf) * fNear;

			float fFarX2 = fWorldX + glm::cos(fWorldA + fFoVHalf) * fFar;
			float fFarY2 = fWorldY + glm::sin(fWorldA + fFoVHalf) * fFar;

			float fNearX2 = fWorldX + glm::cos(fWorldA + fFoVHalf) * fNear;
			float fNearY2 = fWorldY + glm::sin(fWorldA + fFoVHalf) * fNear;

			glm::vec4 ffar = { fFarX1, fFarY1, fFarX2, fFarY2 };
			glm::vec4 fnear = { fNearX1, fNearY1, fNearX2, fNearY2 };

			fWorldX -= glm::cos(fWorldA) * 0.2f * dt;
			fWorldY -= glm::sin(fWorldA) * 0.2f * dt;

			m_Framebuffer->Bind();
			glClearColor(0, 0, 0, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glDisable(GL_DEPTH_TEST);
			m_endingShader->Activate();
			glActiveTexture(GL_TEXTURE0);
			s_endingTexture->Bind();
			glActiveTexture(GL_TEXTURE1);
			s_skyTexture->Bind();
			glActiveTexture(GL_TEXTURE2);
			s_playerTexture->Bind();
			glUniform2f(glGetUniformLocation(m_endingShader->ID, "playerPos"), fWorldX, fWorldY);
			glUniform1f(glGetUniformLocation(m_endingShader->ID, "playerAngle"), fWorldA);
			glUniform1f(glGetUniformLocation(m_endingShader->ID, "cameraHeight"), 1.0f);
			glUniform2f(glGetUniformLocation(m_endingShader->ID, "cameraPos"), fWorldX, fWorldY);
			glUniform1f(glGetUniformLocation(m_endingShader->ID, "cameraAngle"), fWorldA);
			glUniform1f(glGetUniformLocation(m_endingShader->ID, "fFoVHalf"), fFoVHalf);
			glUniform1f(glGetUniformLocation(m_endingShader->ID, "fNear"), fNear);
			glUniform1f(glGetUniformLocation(m_endingShader->ID, "fFar"), fFar);
			glUniform1i(glGetUniformLocation(m_endingShader->ID, "catTexture"), 2);
			glUniform1i(glGetUniformLocation(m_endingShader->ID, "skyTexture"), 1);
			glUniform1i(glGetUniformLocation(m_endingShader->ID, "floorTexture"), 0);
			glUniform4f(glGetUniformLocation(m_endingShader->ID, "far"), ffar.x, ffar.y, ffar.z, ffar.w);
			glUniform4f(glGetUniformLocation(m_endingShader->ID, "near"), fnear.x, fnear.y, fnear.z, fnear.w);
			//glUniform1f(glGetUniformLocation(m_endingShader->ID, "time"), timer);
			glUniform1f(glGetUniformLocation(m_endingShader->ID, "width"), m_Window->GetWidth());
			glUniform1f(glGetUniformLocation(m_endingShader->ID, "height"), m_Window->GetHeight());
			glBindVertexArray(rectVAO);
			glDisable(GL_DEPTH_TEST);
			glDisable(GL_CULL_FACE);

			glDrawArrays(GL_TRIANGLES, 0, 6);
			Camera.Position = glm::vec3(0, 0, -10);
			Renderer2D::BeginScene(Camera.Matrix(90, 0.1f, 100.0f), Camera.Position);

			glEnable(GL_DEPTH_TEST);
			glActiveTexture(GL_TEXTURE0);
			

			glDisable(GL_DEPTH_TEST);

			m_Framebuffer->Unbind(m_Window->GetWidth(), m_Window->GetHeight());
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			m_FramebufferShader->Activate();
			glBindVertexArray(rectVAO);
			glDisable(GL_DEPTH_TEST);
			glDisable(GL_CULL_FACE);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_Framebuffer->GetColorAttachment());
			glDrawArrays(GL_TRIANGLES, 0, 6);

			m_Window->OnUpdate();
			return;
		}

		m_Framebuffer->Bind();
		glClearColor(red, green, blue, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

		if (timer2 >= 1)
		{
			m_Audio->SetVolume(0.5f);
			m_Audio->SetLoop(true);
			m_Audio->Play();

			

			//timer2 = 0;
		}
		else
		{
			timer2 += dt;
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
		glUniform3f(glGetUniformLocation(m_Shader->ID, "camPos"), Camera.Position.x, Camera.Position.y, Camera.Position.z);
		Camera.Matrix(90, 0.01f, 100.0f, m_Shader, "camMatrix");

		// Outputs the matrices into the Vertex Shader
		int modelLoc = glGetUniformLocation(m_Shader->ID, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		// Bind texture and set uniform for scale
		GLuint uniID = glGetUniformLocation(m_Shader->ID, "scale");
		glUniform1f(uniID, 1);

		Renderer2D::BeginScene(Camera.Matrix(90, 0.1f, 100.0f), Camera.Position);

		for (auto& func : m_UpdateFuncList) {
			func(dt, m_Manager);
		}

		for (auto [e, pos, renderer] : m_Manager.EntitiesWith<Components::TransformComponent, Components::Renderer>()) {
			glm::vec3 position = pos.Position;
			glm::vec3 rotation = pos.Rotation;
			glm::vec3 scale = pos.Scale;
			glm::mat4 transform = Math::ComposeTransform(
				position,
				scale,
				rotation
			);

			if (renderer.texture)
			{
				Renderer2D::DrawQuad(transform, renderer.texture, renderer.colour);
			}
			else
			{
				Renderer2D::DrawQuad(transform, renderer.colour);
			}

			//Renderer2D::DrawQuad(transform, renderer.colour);
		}

		for (auto [e, rb2d, pos, box] : m_Manager.EntitiesWith<Components::Rigidbody2DComponent, Components::TransformComponent, Components::BoxCollider2DComponent>())
		{
			b2Body* body = (b2Body*)rb2d.RuntimeBody.get();

			auto position = body->GetPosition();
			position.x = pos.Position.x;
			position.y = pos.Position.y;
			body->ApplyForceToCenter(b2Vec2(0, -.5f), true);
			body->SetTransform(position, glm::radians(pos.Rotation.z));
			body->DestroyFixture(body->GetFixtureList());
			b2PolygonShape boxShape;

			boxShape.SetAsBox(pos.Scale.x * box.Size.x, pos.Scale.y * box.Size.y);
			b2FixtureDef fixtureDef;
			fixtureDef.shape = &boxShape;
			fixtureDef.density = box.Density;
			fixtureDef.friction = box.Friction;
			fixtureDef.restitution = box.Friction;
			fixtureDef.restitutionThreshold = box.RestitutionThreshold;
			body->CreateFixture(&fixtureDef);

		}

		for (auto& rb2d : m_RigidBodiesToRemove)
		{
			b2Body* body = (b2Body*)rb2d.RuntimeBody.get();
			m_World->DestroyBody(body);
		}

		m_RigidBodiesToRemove.erase(m_RigidBodiesToRemove.begin(), m_RigidBodiesToRemove.end());

		const int32_t velocityIterations = 6;
		const int32_t positionIterations = 2;
		m_World->Step(dt, velocityIterations, positionIterations);

		for (auto [e, rb2d, pos] : m_Manager.EntitiesWith<Components::Rigidbody2DComponent, Components::TransformComponent>())
		{
			b2Body* body = (b2Body*)rb2d.RuntimeBody.get();

			const auto& position = body->GetPosition();
			pos.Position.x = position.x;
			pos.Position.y = position.y;
			pos.Rotation.z = glm::degrees(body->GetAngle());
		}

		m_Manager.Refresh();

		/*transform = Math::ComposeTransform(
			glm::vec3(0.0f, 2, 0.0f),
			glm::vec3(2.0f, 2.0f, 1.0f),
			glm::vec3(0.0f, 0.0f, rotation)
		);

		Renderer2D::DrawQuad(transform, m_Texture, glm::vec4(1.0f));*/

		Renderer2D::EndScene();
		
		m_Framebuffer->Unbind(m_Window->GetWidth(), m_Window->GetHeight());
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		m_FramebufferShader->Activate();
		glBindVertexArray(rectVAO);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		glBindTexture(GL_TEXTURE_2D, m_Framebuffer->GetColorAttachment());
		glDrawArrays(GL_TRIANGLES, 0, 6);

		m_Window->OnUpdate();

		m_fistFime = false;
	}

	void Application::ResizeGL(uint32_t w, uint32_t h)
	{
		m_Window->ResizeGL(w, h);
		m_Framebuffer->Resize(w, h);
	}
	
}
