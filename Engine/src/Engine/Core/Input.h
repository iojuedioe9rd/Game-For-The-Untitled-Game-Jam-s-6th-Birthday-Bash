#pragma once
#include "Engine/Core/Base.h"


#define ENG_KEYS_MAX 256
namespace Engine
{
	class ENGINE_API Input
	{
	public:
		static bool IsPressed(char key);
		static void SetPressed(char key, bool pressed);

	private:

		static void Init();
		// ENG_KEYS_MAX
		static bool keys[ENG_KEYS_MAX];

		friend class Application;
	};
}