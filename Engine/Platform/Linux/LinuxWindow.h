#pragma once

#include "Engine/Core/Window.h"

#include <SDL3/SDL.h>

namespace Engine
{

	class ENGINE_API LinuxWindow : public Window
	{
	public:
		LinuxWindow(const WindowProps& props);
		virtual ~LinuxWindow() override;

		virtual void Show() override;
		virtual void Hide() override;

		virtual int GetWidth() override;
		virtual int GetHeight() override;

		virtual void* GetRaw() override;
		virtual void SetVSync(bool enabled) override;
		virtual bool IsVSync() const override;

		virtual void OnUpdate() override;
	private:
		virtual void Init(const WindowProps& props);
		virtual void Shutdown();
	private:
		

		struct WindowData
		{
			SDL_Window* Window;
			std::string Title;
			unsigned int Width, Height;
			bool VSync;

			//EventCallbackFn EventCallback;
		};

		WindowData m_Data;
	};
}