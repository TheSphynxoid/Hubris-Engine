#include "pch.h"
#include "Core/Graphics/Window.h"
#ifdef SPH_WINDOWS
#include "Core/Graphics/Vulkan/VkWindow.h"
#endif

void Sphynx::Graphics::Window::Create(const glm::ivec2& size, const char* title)
{
#ifdef SPH_WINDOWS
    // auto m_Window = new Vulkan::vkWindow(size, title);
#endif
}
