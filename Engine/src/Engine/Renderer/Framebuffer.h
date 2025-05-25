#pragma once

#include <Engine/Core/Base.h>

namespace Engine {

	class ENGINE_API Framebuffer
	{
	public:
		static Ref<Framebuffer> Create(int width, int height)
		{
			return CreateRef<Framebuffer>(width, height);
		}

		Framebuffer(int width, int height);
		~Framebuffer();
		void Bind();
		void Unbind(uint32_t windowWidth, uint32_t windowHeight);
		void Resize(int width, int height);
		uint32_t GetColorAttachment() const { return m_ColorAttachment; }
		uint32_t GetDepthAttachment() const { return m_DepthAttachment; }
		int GetWidth() const { return m_Width; }
		int GetHeight() const { return m_Height; }
	private:
		uint32_t m_FramebufferID;
		uint32_t m_ColorAttachment;
		uint32_t m_DepthAttachment;
		int m_Width, m_Height;
	};
}