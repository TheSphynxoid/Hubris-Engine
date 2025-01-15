#include "pch.h"
#include "Core/Graphics/Vulkan/vkWindow.h"
#include "GLFW/glfw3.h"

struct Sphynx::Graphics::Vulkan::vkWindow::Details {
	GLFWwindow* Window;
};

Sphynx::Graphics::Vulkan::vkWindow::vkWindow(int Width, int Height, const std::string& title)
{
	glfwInitHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	if(glfwInit() == GLFW_FALSE){
		Logger::Log("Failed to initialize GLFW");
		return;
	}
	details = new Details();
	details->Window = glfwCreateWindow(Width, Height, title.c_str(), nullptr, nullptr);
	glfwShowWindow(details->Window);
}

Sphynx::Graphics::Vulkan::vkWindow::~vkWindow()
{
	Close();
	delete details;
}

void Sphynx::Graphics::Vulkan::vkWindow::Init()
{

}

void Sphynx::Graphics::Vulkan::vkWindow::Update() noexcept
{
	while (!glfwWindowShouldClose(details->Window)) {
		glfwPollEvents();
	}
}
