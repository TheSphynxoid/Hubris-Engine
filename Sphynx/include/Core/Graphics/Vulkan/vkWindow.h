#pragma once
#include <Core/Graphics/Window.h>

namespace Sphynx::Graphics::Vulkan {
	class vkWindow : public Window {
	private:
		struct Details;
		Details* details= nullptr;
	public:
		vkWindow(int Width, int Height, const std::string& title);
		virtual ~vkWindow() override;
		void Init() override;
		void Update()noexcept override;
		void Close()noexcept override = 0;
		Viewport GetViewport()const noexcept override = 0;
		bool IsValid()const noexcept override = 0;
	};
}