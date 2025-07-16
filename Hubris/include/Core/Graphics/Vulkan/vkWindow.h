#pragma once
#include <Core/Graphics/Window.h>
#include <Engine.h>
#include "Core/Graphics/Vulkan/Utility.h"
#include <Core/Graphics/Vulkan/vkSwapchain.h>

namespace Hubris::Graphics::Vulkan {
	/**
	 * @brief Vulkan specific Windowing implementation.
	 */
	class vkWindow final : public Window {
	private:
		friend class Engine;
		/**
		 * @brief This opaque struct contains implementation details to avoid exposing vulkan to user codebase.
		 */
		struct Details;
		Details* details= nullptr;
		vkSwapchain swapchain = vkSwapchain(VK_NULL_HANDLE, (VkFormat)-1, VkExtent2D());
		vkWindow() = default;
		static void InitGLFW();
	public:
		[[nodiscard]] static vkWindow* Create(int Width, int Height, const std::string& title);
		virtual ~vkWindow() override;
		void Init() override;
		void Update() noexcept override;
		void Close() noexcept override;
		Viewport GetViewport() const noexcept override;
		bool IsValid() const noexcept override;
		inline bool IsRunning()const noexcept override;
		void* GetNative() const noexcept override;
		void* GetSurface()const noexcept;
		Swapchain* GetSwapchain()const noexcept;
	};
}