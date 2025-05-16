#include "Input.h"

namespace Engine
{
	bool Input::keys[ENG_KEYS_MAX] = {0};

	void Input::Init()
	{
		
	}

	bool Input::IsPressed(char key)
	{
		return keys[key];
	}

	void Input::SetPressed(char key, bool pressed)
	{
		keys[key] = pressed;
	}
	
}