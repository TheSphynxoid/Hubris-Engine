#pragma once
#include <Logger.h>
#define VOLK_IMPLEMENTATION 
#include <volk.h>
#include <GLFW/glfw3.h>
#include "Engine.h"

namespace Sphynx::Graphics::Vulkan {
    //A struct to hold the device information and Important features availability
    struct Device{
        uint32_t DeviceID;
        uint32_t VendorID;
        uint32_t APIVersion;
        uint32_t DriverVersion;
        
        unsigned int Score = 0;
        bool RayTracingCapable = false;
        bool GeometryShader = false;
        bool TessellationShader = false;
        bool ShaderInt64 = false;
        bool DiscreteGPU = false;
        bool SparceBinding = false;
        std::vector<int> QueueFamilyIndices;
    };
    class VulkanBackend {
    private:
        static inline VkInstance instance;
        static inline VkPhysicalDevice physicalDevice;
        static inline VkDevice device;
        static Device ScoreGPU(const VkPhysicalDeviceProperties& prop, const VkPhysicalDeviceFeatures& features, const std::vector<VkExtensionProperties>& ext) {
            Device device;
            device.GeometryShader = features.geometryShader;
            device.TessellationShader = features.tessellationShader;
            device.DiscreteGPU = prop.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
            device.ShaderInt64 = features.shaderInt64;
            device.SparceBinding = features.sparseBinding;
            device.APIVersion = prop.apiVersion;
            

            static const std::vector<const char*> requiredRTExtensions = {
                // Required ray tracing extensions
                VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME,
                VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME,
                VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME,
                VK_KHR_SPIRV_1_4_EXTENSION_NAME
            };

            if(!device.GeometryShader){
                Logger::Log("Device does not support Geometry Shader");
                return device;
            }

            if (prop.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
                device.Score += 1000;
            }
            
            int found = 0;
            for(const auto& RText : requiredRTExtensions){
                for(const auto& supportedExt : ext){
                    if(strcmp(RText, supportedExt.extensionName) == 0){
                        found++;
                        break;
                    }
                }
            }
            if(found == requiredRTExtensions.size()){
                device.RayTracingCapable = true;
                device.Score += 1000;
            }

            return device;
        }
    public:
        static void Init()noexcept {
            if (volkInitialize() != VK_SUCCESS) {
                Logger::Log("Unable to initialize volk");
            }

            uint32_t extensionCount = 0;
            vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

            std::cout << extensionCount << " extensions supported\n";

            VkApplicationInfo appInfo{};
            appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
            appInfo.pApplicationName = Engine::GetProjectName().data();
            const Version& pVersion = Engine::GetProjectVersion();
            appInfo.applicationVersion = VK_MAKE_API_VERSION(pVersion.Variant, pVersion.Major, pVersion.Minor, pVersion.Patch);
            appInfo.pEngineName = "Sphynx Engine";
            appInfo.engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
            appInfo.apiVersion = VK_API_VERSION_1_3;
                
            VkInstanceCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
            createInfo.pApplicationInfo = &appInfo;
            createInfo.enabledExtensionCount = extensionCount;

            uint32_t glfwExtensionCount = 0;
            const char** glfwExtensions;

            glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

            createInfo.enabledExtensionCount = glfwExtensionCount;
            createInfo.ppEnabledExtensionNames = glfwExtensions;

            createInfo.enabledLayerCount = 0;

            if(vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS){
                Logger::Log("Failed to create vulkan instance");
                return;
            }
            Logger::Log("Vulkan Instance Created");

            volkLoadInstance(instance);

            QueryDevices();
        }
        /**
         * @brief Queries the available devices and scores them based on their features, then caches the Query.
         * This function is called if a device info cache is not found.
         */
        static std::multimap<int, Device> QueryDevices(){
            uint32_t deviceCount = 0;
            vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

            if(deviceCount == 0){
                Logger::Log("Failed to find GPUs with Vulkan support. Verify that your driver supports Vulkan");
                return std::multimap<int, Device>();
            }

            std::vector<VkPhysicalDevice> devices(deviceCount);
            vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
            std::multimap<int, Device> deviceMap;

            std::fstream file("GPUDevice.cache", std::ios::out | std::ios::binary);
            if(file.is_open()){
                file.write((char*)&deviceCount, sizeof(size_t));
            }

            for(const auto& device : devices){
                VkPhysicalDeviceProperties deviceProperties;
                vkGetPhysicalDeviceProperties(device, &deviceProperties);

                Logger::Log("(vk)Found Device : " + std::string(deviceProperties.deviceName));

                VkPhysicalDeviceFeatures deviceFeatures;
                vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

                uint32_t extensionCount;
                vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
                std::vector<VkExtensionProperties> extensions(extensionCount);
                vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, extensions.data());

                Device d = ScoreGPU(deviceProperties, deviceFeatures, extensions);

                //Queue Family Scoring.
                uint32_t queueFamilyCount = 0;
                vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

                std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
                vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

                for(const auto& queueFamily : queueFamilies){
                    Logger::Log("Device Queue Families Flags: {}{}{}{}", queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT ? "Graphics " : "", 
                        queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT ? "Compute " : "",
                        queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT ? "Transfer " : "",
                        queueFamily.queueFlags & VK_QUEUE_SPARSE_BINDING_BIT ? "Sparce " : "");
                    Logger::Log("Queue Count: {}", queueFamily.queueCount);
                    if(queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT){
                        d.Score += 100;//Graphics is a general purpose feature.
                    }
                    if(queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT){
                        d.Score += 150;//Compute is a beneficial feature.
                    }
                    if(queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT){
                        d.Score += 150;//Transfer allows for more efficient data transfer.
                    }
                    if(queueFamily.queueFlags & VK_QUEUE_SPARSE_BINDING_BIT){
                        d.Score += 50;//Arbitrarily chosen.
                    }
                }//Queue Family Scoring
                deviceMap.insert(std::make_pair(d.Score,d));

                //Cache the scored devices.
                if(file.is_open()){
                    file.write((char*)&d, sizeof(Device));
                    file.close();
                }
            }//Device Scoring
            return deviceMap;
        }
    };
}