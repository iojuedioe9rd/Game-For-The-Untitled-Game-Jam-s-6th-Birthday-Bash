#pragma once
#include "Engine/Core/Base.h"


namespace Engine
{
	class ENGINE_API Audio
	{
	public:

		static void Init();
		static void Shutdown();
		static Ref<Audio> Create(const char* filePath);

		void Play();
		void Stop();
		void SetVolume(float volume);
		void SetPitch(float pitch);
		void SetPan(float pan);
		void SetLoop(bool loop);

		Audio(const char* filePath);
		~Audio();

	private:
		void* m_RuntimeData;
	};

	
}