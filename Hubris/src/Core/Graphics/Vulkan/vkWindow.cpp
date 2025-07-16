#include "pch.h"
#include "Core/Graphics/Vulkan/vkWindow.h"
#define VOLK_IMPLEMENTATION 
#include "Core/Graphics/Vulkan/vkBackend.h"
// #include "GLFW/glfw3.h"

/**
 * @internal
 * @private
 */
struct Hubris::Graphics::Vulkan::vkWindow::Details {
	GLFWwindow* Window;
	VkSurfaceKHR surface;
};

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

//TODO: Make the user able to choose for their game.
VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) noexcept
{
	for (const auto& availableFormat : availableFormats) {
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return availableFormat;
		}
	}
	
	return availableFormats[0];
}

VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
    for (const auto& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}


VkExtent2D ChooseSwapExtent(GLFWwindow* window, const VkSurfaceCapabilitiesKHR& capabilities) {
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    } else {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        VkExtent2D actualExtent = {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };

        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }
}

void Hubris::Graphics::Vulkan::vkWindow::InitGLFW()
{
	if(glfwInit() == GLFW_FALSE){
		Logger::Log("Failed to initialize GLFW");
		return;
	}

	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
}

Hubris::Graphics::Vulkan::vkWindow* Hubris::Graphics::Vulkan::vkWindow::Create(int Width, int Height, const std::string& title)
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

	VkPhysicalDevice pdevice = vkBackend::GetPhysicalDevice();
	//Creating the swapchain.
	SwapChainSupportDetails swapDetails;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(pdevice, surface, &swapDetails.capabilities);
	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(pdevice, surface, &formatCount, nullptr);
	if (formatCount != 0) {
		swapDetails.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(pdevice, surface, &formatCount, swapDetails.formats.data());
	}
	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(pdevice, surface, &presentModeCount, nullptr);

	if (presentModeCount != 0) {
		swapDetails.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(pdevice, surface, &presentModeCount, swapDetails.presentModes.data());
	}

	bool swapChainAdequate = !swapDetails.formats.empty() && !swapDetails.presentModes.empty();

	if(!swapChainAdequate){
		Logger::Fatal("No adequate Swapchain found.");
		vkwindow->Close();
		delete vkwindow;
		return nullptr;
	}


	VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapDetails.formats);
    VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapDetails.presentModes);
    VkExtent2D extent = ChooseSwapExtent(win, swapDetails.capabilities);

	uint32_t imageCount = swapDetails.capabilities.minImageCount + 1;

	if (swapDetails.capabilities.maxImageCount > 0 && imageCount > swapDetails.capabilities.maxImageCount) {
		imageCount = swapDetails.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = surface;

	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	createInfo.preTransform = swapDetails.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;

	if (vkRenderer::GetGraphicsQueue() != vkRenderer::GetPresentQueue()) {
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		static uint32_t queueFamilyIndices[] = { vkRenderer::GetGraphicsQueueIndex(), vkRenderer::GetPresentQueueIndex() };
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	} else {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0; // Optional
		createInfo.pQueueFamilyIndices = nullptr; // Optional
	}

	VkSwapchainKHR swapchain;

	if (vkCreateSwapchainKHR(vkBackend::GetDevice(), &createInfo, nullptr, &swapchain) != VK_SUCCESS) {
		Logger::Fatal("failed to create swap chain!");
		throw std::runtime_error("failed to create swap chain!");
	}

	//Get the count from vulkan now.
	vkwindow->swapchain = vkSwapchain(swapchain, surfaceFormat.format, extent);


	return vkwindow;
}

Hubris::Graphics::Vulkan::vkWindow::~vkWindow()
{
	Close();
	delete details;
}

void Hubris::Graphics::Vulkan::vkWindow::Init()
{
	//TODO: add Hooks to the window.
	
}

void Hubris::Graphics::Vulkan::vkWindow::Update() noexcept
{
	while (!glfwWindowShouldClose(details->Window)) {
		glfwPollEvents();
	}
}

void Hubris::Graphics::Vulkan::vkWindow::Close() noexcept
{
	glfwSetWindowShouldClose(details->Window, true);
	vkDestroySurfaceKHR(vkBackend::GetInstance(), details->surface, nullptr);
	swapchain.Destroy();
}

Hubris::Graphics::Viewport Hubris::Graphics::Vulkan::vkWindow::GetViewport() const noexcept
{
    return Viewport();
}

bool Hubris::Graphics::Vulkan::vkWindow::IsValid() const noexcept
{
    return glfwWindowShouldClose(details->Window);
}

bool Hubris::Graphics::Vulkan::vkWindow::IsRunning() const noexcept 
{
	return !glfwWindowShouldClose(details->Window);
}

void* Hubris::Graphics::Vulkan::vkWindow::GetNative() const noexcept
{
    return (void*)details->Window;
}

void* Hubris::Graphics::Vulkan::vkWindow::GetSurface()const noexcept
{
	return (void*)details->surface;
}

Hubris::Graphics::Swapchain* Hubris::Graphics::Vulkan::vkWindow::GetSwapchain() const noexcept
{
	return (Swapchain*)&swapchain;
}
