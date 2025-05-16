#include <cstdlib>
#include <cstdio>
#include "../../Engine/src/Engine/main.h"
#include "../../Engine/src/Engine/Core/Application.h"
#include "Engine/Core/Input.h"

#define SDL_MAIN_USE_CALLBACKS 1  /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>


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

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
	SDL_SetAppMetadata("Example Renderer Clear", "1.0", "com.example.renderer-clear");

	s_Game = (Game*)Engine::CreateApplication();
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

