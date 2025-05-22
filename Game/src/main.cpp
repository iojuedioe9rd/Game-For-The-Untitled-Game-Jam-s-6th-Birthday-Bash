#include <cstdlib>
#include <cstdio>
#include "../../Engine/src/Engine/main.h"
#include "../../Engine/src/Engine/Core/Application.h"
#include "Engine/Core/Input.h"

#define SDL_MAIN_USE_CALLBACKS 1  /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <Engine/Core/Components.h>


/* We will use this renderer to draw into this window every frame. */
static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;

class Game : public Engine::Application
{
public:
	Game()
	{

	}
	~Game()
	{

	}

private:

};

namespace Engine
{
	Application* CreateApplication()
	{
		return new Game();
	}
}

static Game* s_Game = nullptr;

struct Player
{
	float speed = 5.0f;
};

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
	SDL_SetAppMetadata("Example Renderer Clear", "1.0", "com.example.renderer-clear");

	s_Game = (Game*)Engine::CreateApplication();
	s_Game->AddUpdateFunc([&](float dt, ecs::Manager& m) {
		// Update the game state here

		for (auto [e, player, tr] : m.EntitiesWith<Player, Engine::Components::TransformComponent>())
		{
			glm::vec2 direction = glm::vec2(0, 0);
			// player.speed * dt
			/*
			if (Engine::Input::IsPressed('w'))
			{
				direction.y += 1;
			}

			if (Engine::Input::IsPressed('s'))
			{
				direction.y -= 1;
			}*/

			if (Engine::Input::IsPressed('a'))
			{
				direction.x -=1;
			}

			if (Engine::Input::IsPressed('d'))
			{
				direction.x += 1;
			}

			if (direction.x == 0 && direction.y == 0)
			{
				continue;
			}
			direction = glm::normalize(direction);

			tr.Position += glm::vec3(direction.x, direction.y, 0) * player.speed * dt;
		}
	});
	ecs::Entity player = s_Game->GetManager().CreateEntity();
	player.Add<Engine::Components::TransformComponent>(glm::vec3(0, 1, 0), glm::vec3(0), glm::vec3(2));
	player.Add<Engine::Components::Renderer>(glm::vec4(1, 1, 1, 1));
	s_Game->NewCommponent<Engine::Components::Rigidbody2DComponent>(player, Engine::Components::Rigidbody2DComponent::BodyType::Dynamic);
	s_Game->NewCommponent<Engine::Components::BoxCollider2DComponent>(player);
	player.Add<Player>();

	ecs::Entity floor = s_Game->GetManager().CreateEntity();
	floor.Add<Engine::Components::TransformComponent>(glm::vec3(-5), glm::vec3(0), glm::vec3(100, 1, 1));
	floor.Add<Engine::Components::Renderer>(glm::vec4(1, 1, 1, 1));
	s_Game->NewCommponent<Engine::Components::Rigidbody2DComponent>(floor, Engine::Components::Rigidbody2DComponent::BodyType::Static);
	Engine::Components::BoxCollider2DComponent::BoxCollider2DSettings settings = {};
	settings.Density = 1.0f;
	settings.Friction = 0.5f;
	settings.Restitution = 0.0f;
	settings.RestitutionThreshold = 0.5f;
	s_Game->NewCommponent<Engine::Components::BoxCollider2DComponent>(floor, settings);



	s_Game->GetManager().Refresh();

	s_Game->Run();
	



	return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{
	if (event->type == SDL_EVENT_QUIT) {
		return SDL_APP_SUCCESS;  /* end the program, reporting success to the OS. */
	}

	if (event->type == SDL_EVENT_KEY_DOWN)
	{
		Engine::Input::SetPressed(event->key.key, true);
		//event->key.key
	}

	if (event->type == SDL_EVENT_KEY_UP)
	{
		Engine::Input::SetPressed(event->key.key, false);
		//event->key.key
	}

	return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void* appstate)
{
	s_Game->Update();

	return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
	/* SDL will clean up the window/renderer for us. */
}

