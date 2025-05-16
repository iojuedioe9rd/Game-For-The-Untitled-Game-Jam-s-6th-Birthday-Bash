#pragma once
#include "Engine/Core/Base.h"

namespace Engine
{
	struct ENGINE_API WindowProps
	{
		std::string Title;
		unsigned int Width;
		unsigned int Height;

		WindowProps(const std::string& title = "Game",
			unsigned int width = 1280,
			unsigned int height = 720)
			: Title(title), Width(width), Height(height)
		{
		}
	};

	class ENGINE_API Window
	{
	public:
		static Ref<Window> New(const WindowProps& props = WindowProps());

		Window(const WindowProps& props);
		virtual ~Window();

		virtual void Show() = 0;
		virtual void Hide() = 0;

		virtual int GetWidth() = 0;
		virtual int GetHeight() = 0;

		virtual void* GetRaw() = 0;
		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVSync() const = 0;
		void ResizeGL(uint32_t w, uint32_t h);

		virtual void OnUpdate() = 0;
	};
}