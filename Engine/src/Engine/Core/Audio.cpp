#include "Audio.h"
#include "miniaudio.h"

namespace Engine
{
	static ma_engine* s_Engine = nullptr;

	void Audio::Init()
	{
		if (s_Engine == nullptr)
		{
			s_Engine = new ma_engine();
			ma_result result = ma_engine_init(NULL, s_Engine);
			if (result != MA_SUCCESS)
			{

				// Handle error
			}
		}
		else
		{
			return;
		}


	}

	void Audio::Shutdown()
	{
		if (s_Engine != nullptr)
		{
			ma_engine_uninit(s_Engine);
			delete s_Engine;
			s_Engine = nullptr;
		}
		else
		{
			return;
		}
	}

	Ref<Audio> Audio::Create(const char* filePath)
	{
		return CreateRef<Audio>(filePath);
	}

	void Audio::Play()
	{
		ma_sound_start((ma_sound*)m_RuntimeData);
	}

	void Audio::Stop()
	{
		ma_sound_stop((ma_sound*)m_RuntimeData);
	}

	void Audio::SetVolume(float volume)
	{
		ma_sound_set_volume((ma_sound*)m_RuntimeData, volume);
	}

	void Audio::SetPitch(float pitch)
	{
		ma_sound_set_pitch((ma_sound*)m_RuntimeData, pitch);
	}

	void Audio::SetPan(float pan)
	{
		ma_sound_set_pan((ma_sound*)m_RuntimeData, pan);
	}

	void Audio::SetLoop(bool loop)
	{
		ma_sound_set_looping((ma_sound*)m_RuntimeData, loop);
	}

	Audio::Audio(const char* filePath)
	{
		m_RuntimeData = nullptr;
		ma_sound* sound = (ma_sound*)malloc(sizeof(ma_sound));

		ma_result result = ma_sound_init_from_file(s_Engine, filePath, 0, NULL, NULL, sound);

		if (result != MA_SUCCESS)
		{
			const char* errorMessage = ma_result_description(result);
			printf("Error loading audio file: %s\n", errorMessage);
			// Handle error
			
		}
		else
		{
			m_RuntimeData = (void*)sound;
		}


	}

	Audio::~Audio()
	{
		if (m_RuntimeData != nullptr)
		{
			ma_sound_uninit((ma_sound*)m_RuntimeData);
			m_RuntimeData = nullptr;
		}
		else
		{
			return;
		}
	}
}