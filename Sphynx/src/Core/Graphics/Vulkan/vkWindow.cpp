#include "pch.h"
#include "Core/Graphics/Vulkan/vkWindow.h"
#include "GLFW/glfw3.h"

Sphynx::Graphics::Vulkan::vkWindow::vkWindow(int Width, int Height, std::string title)
{
	glfwInitHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

}