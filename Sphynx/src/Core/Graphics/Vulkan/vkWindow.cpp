#include "pch.h"
#include "Core/Graphics/Vulkan/vkWindow.h"
#define VOLK_IMPLEMENTATION 
#include "Core/Graphics/Vulkan/vkBackend.h"
// #include "GLFW/glfw3.h"

/**
 * @internal
 * @private
 */
struct Sphynx::Graphics::Vulkan::vkWindow::Details {
	GLFWwindow* Window;
	VkSurfaceKHR surface;
};

void Sphynx::Graphics::Vulkan::vkWindow::InitGLFW()
{
	if(glfwInit() == GLFW_FALSE){
		Logger::Log("Failed to initialize GLFW");
		return;
	}

	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
}

Sphynx::Graphics::Vulkan::vkWindow* Sphynx::Graphics::Vulkan::vkWindow::Create(int Width, int Height, const std::string& title)
{		
	GLFWwindow* win;
	win = glfwCreateWindow(Width, Height, title.c_str(), nullptr, nullptr);
	if(!win){
		const char* error;
		int n = glfwGetError(&error);
		Logger::Fatal("Error Creating Window: {}", error);
	}
	glfwShowWindow(win);
	VkSurfaceKHR surface;
    if (glfwCreateWindowSurface(vkBackend::GetInstance(), win, nullptr, &surface) != VK_SUCCESS) {
		Logger::Fatal("Failed to create a window surface.");
		const char* desc;
		glfwGetError(&desc);
		Logger::Fatal("GLFW: {}", desc);
		return nullptr;
    }
	vkWindow* vkwindow = new vkWindow();
	vkwindow->details = new Details();
	vkwindow->details->Window = win;
	vkwindow->details->surface = surface;
	glfwSetWindowUserPointer(win, vkwindow->details);
	vkBackend::Init(vkwindow->details->surface);

	return vkwindow;
}

Sphynx::Graphics::Vulkan::vkWindow::~vkWindow()
{
	Close();
	delete details;
}

void Sphynx::Graphics::Vulkan::vkWindow::Init()
{
	//TODO: add Hooks to the window.
	
}

void Sphynx::Graphics::Vulkan::vkWindow::Update() noexcept
{
	while (!glfwWindowShouldClose(details->Window)) {
		glfwPollEvents();
	}
}

void Sphynx::Graphics::Vulkan::vkWindow::Close() noexcept
{
	glfwSetWindowShouldClose(details->Window, true);
	vkDestroySurfaceKHR(vkBackend::GetInstance(), details->surface, nullptr);
}

Sphynx::Graphics::Viewport Sphynx::Graphics::Vulkan::vkWindow::GetViewport() const noexcept
{
    return Viewport();
}

bool Sphynx::Graphics::Vulkan::vkWindow::IsValid() const noexcept
{
    return glfwWindowShouldClose(details->Window);
}

bool Sphynx::Graphics::Vulkan::vkWindow::IsRunning() const noexcept 
{
	return !glfwWindowShouldClose(details->Window);
}


void *Sphynx::Graphics::Vulkan::vkWindow::GetNative() const noexcept
{
    return (void*)details->Window;
}