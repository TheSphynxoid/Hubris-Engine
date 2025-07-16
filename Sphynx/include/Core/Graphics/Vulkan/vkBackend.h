#pragma once
#include <Logger.h>
#include "volk.h"
#include <GLFW/glfw3.h>
#include "Engine.h"
#include "vkRenderer.h"

namespace Sphynx::Graphics::Vulkan {

    #ifdef NDEBUG
    const bool enableValidationLayers = false;
    #else
    const bool enableValidationLayers = true;
    #endif
    /**
     * @brief A struct to hold the device information and Important features availability.
     * It's mainly used to cache information for faster %Engine startup.
     */
    struct Device{
        uint32_t DeviceID = 0; ///< The %Device ID, used for caching. @hideinitializer
        uint32_t VendorID = 0; ///< The Vendor ID, used for caching. @hideinitializer
        uint32_t APIVersion = 0; ///< %Vulkan API version, used for cache validation. @hideinitializer
        uint32_t DriverVersion = 0; ///< The Driver version, used for cache validation. @hideinitializer
        
        unsigned int Score = 0; ///< The Computed score of the device. @hideinitializer
        bool ExtSupported = false; ///< Indicates if the Device can be used. @hideinitializer
        bool RayTracingCapable = false; ///< Is the %Device Capable of Raytracing. @hideinitializer
        bool GeometryShader = false; ///< Can Geometry Shaders be used ? This is a required feature. @hideinitializer
        bool TessellationShader = false;  ///< Can Tesselation Shaders be used ? @hideinitializer
        bool ShaderInt64 = false; ///<  @hideinitializer
        bool DiscreteGPU = false; ///< Is the %Device a Discrete (Dedicated) GPU. @hideinitializer
        bool SparceBinding = false; ///< Is the %Device capable of Sparce Binding. @hideinitializer
    };
    /**
     * @brief Basic info about a QueueFamily
     *
     */
    struct QueueFamily {
        std::optional<uint32_t> Index;
        uint32_t Count = 0;
        uint32_t PresentSupport;
    };
    /**
     * @brief The Vulkan backend manager.
     */
    class vkBackend final {
    private:
        /**
         * @brief This struct is used for initializing device.
         * Do not cache because most of the data can change each run.
         * (For example, vkdevice handle is a runtime object)
         */
        struct RuntimeDeviceData{
            Device device;
            VkDevice vkDevice;
            VkPhysicalDevice vkPhysicalDevice;
            QueueFamily Graphics;
            QueueFamily Compute;
            QueueFamily Present;
            std::vector<QueueFamily> Transfer;
        };

        static inline RuntimeDeviceData SelectedDevice;
        static inline VkDebugUtilsMessengerEXT debugMessenger;

        static inline VkInstance instance = nullptr;
        static inline VkPhysicalDevice physicalDevice = nullptr;
        static inline VkDevice device = nullptr;
        static inline const std::vector<const char*> requiredExt = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
            VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME,
            VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME,
            VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME,
            VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME,
            VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME,
            VK_KHR_SPIRV_1_4_EXTENSION_NAME
        };

        static inline const std::vector<const char*> validationLayers = {
            "VK_LAYER_KHRONOS_validation"
        };

        static VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
            auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
            if (func != nullptr) {
                return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
            } else {
                return VK_ERROR_EXTENSION_NOT_PRESENT;
            }
        }
        
        static void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
            auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
            if (func != nullptr) {
                func(instance, debugMessenger, pAllocator);
            }
        }

        static Device ScoreGPU(const VkPhysicalDeviceProperties& prop, const VkPhysicalDeviceFeatures& features, const std::vector<VkExtensionProperties>& ext) noexcept {
            Device device;
            device.GeometryShader = features.geometryShader;
            device.TessellationShader = features.tessellationShader;
            device.DiscreteGPU = prop.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
            device.ShaderInt64 = features.shaderInt64;
            device.SparceBinding = features.sparseBinding;
            device.APIVersion = prop.apiVersion;
            //This is redundent but is used to detect raytracing specific extension, Remove if possible.
            static const std::vector<const char*> requiredRTExtensions = {
                // Required ray tracing extensions
                VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME,
                VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME,
                VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME,
                VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME,
                VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME,
                VK_KHR_SPIRV_1_4_EXTENSION_NAME
            };

            if(!device.GeometryShader){
                Logger::Log("Device does not support Geometry Shader");
                return device;
            }

            if (prop.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
                device.Score += 1000;
            }

            std::set<std::string> requiredExtensions(requiredExt.begin(), requiredExt.end());
            std::set<std::string> rtxExt(requiredRTExtensions.begin(), requiredRTExtensions.end());
            int found = 0;
            for(const auto& supportedExt : ext){
                requiredExtensions.erase(supportedExt.extensionName);
                rtxExt.erase(supportedExt.extensionName);
            }

            if(rtxExt.empty()){
                device.RayTracingCapable = true;
                device.Score += 1000;
            }
            if(requiredExtensions.empty()){
                device.Score +=1000;
                device.ExtSupported = true;
            }
            return device;
        }

        static void InitLayers() noexcept {
            uint32_t layerCount;
            vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

            Logger::Log("(vk)Found {} Layers", layerCount);

            std::vector<VkLayerProperties> availableLayers(layerCount);
            vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

            for (const char* layerName : validationLayers) {
                bool layerFound = false;

                for (const auto& layerProperties : availableLayers) {
                    if (strcmp(layerName, layerProperties.layerName) == 0) {
                        layerFound = true;
                        break;
                    }
                }

                if (!layerFound) {
                    Logger::Log("(vk)Validation Layers not found");
                }
            }
            Logger::Log("(vk)Validation Layers Found");

        }
    public:
        vkBackend() = delete;
        ~vkBackend() = delete;

        static void CreateInstance() noexcept {
            if(instance){
                Logger::Log("A Vulkan Instance already exists. Attempt to create another instance ignored.");
                return;
            }
            if (volkInitialize() != VK_SUCCESS) {
                Logger::Fatal("Unable to initialize volk");
                return;
            }

            uint32_t extensionCount = 0;
            vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

            std::cout << extensionCount << " extensions supported\n";
            auto& ProjectName = Engine::GetProjectName();
            VkApplicationInfo appInfo{};
            appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
            appInfo.pApplicationName = ProjectName.data();
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
            
            if(!glfwExtensions){
                const char* desc;
                glfwGetError(&desc);
                Logger::Fatal("GLFW: {}", desc);
                return;
            }
            std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
            if (enableValidationLayers) {
                extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
            }
            //According to vulkan specs, size will never overflow. Thus, this has no risk and the warning can be ignored here.
#pragma warning( push )
#pragma warning( disable : 4267)
            createInfo.enabledExtensionCount = extensions.size();
#pragma warning( pop ) 
            createInfo.ppEnabledExtensionNames = extensions.data();

            VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
            if (enableValidationLayers) {
                createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
                createInfo.ppEnabledLayerNames = validationLayers.data();

                populateDebugMessengerCreateInfo(debugCreateInfo);
                createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
            } else {
                createInfo.enabledLayerCount = 0;

                createInfo.pNext = nullptr;
            }

            if(vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS){
                Logger::Log("Failed to create vulkan instance");
                return;
            }
            Logger::Log("Vulkan Instance Created");

            volkLoadInstance(instance);

            setupDebugMessenger();
        }

        static VkInstance GetInstance() noexcept { return instance;};
        static VkDevice GetDevice() noexcept { return device; };
        static VkPhysicalDevice GetPhysicalDevice() noexcept {return physicalDevice;};
        /**
         * @brief Initializes the Vulkan backend.
         * This is the first step to use any vulkan api.
         *
         * @note Don't call this manually, unless GraphicsApi is not managed by the engine.
         */
        static ErrorCode Init(VkSurfaceKHR surface)noexcept {
            auto Devices = QueryDevices(surface);
            if(!Devices.size())return ErrorCode::INTERNAL_ERROR;
            auto bestDev = (*Devices.begin()).second;
            SelectedDevice = bestDev;   
            
            physicalDevice = bestDev.vkPhysicalDevice;

            std::set<uint32_t> UniqueQueueFamilies = { bestDev.Graphics.Index.value(), bestDev.Present.Index.value()};
            std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

            float queuePriority = 1.0f;
            
            for (uint32_t queueFamily : UniqueQueueFamilies) {
                VkDeviceQueueCreateInfo queueCreateInfo{};
                queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
                queueCreateInfo.queueFamilyIndex = queueFamily;
                queueCreateInfo.queueCount = 1;
                queueCreateInfo.pQueuePriorities = &queuePriority;
                queueCreateInfos.push_back(queueCreateInfo);
            }

            VkPhysicalDeviceFeatures deviceFeatures{};
            deviceFeatures.geometryShader = true;

            VkDeviceCreateInfo deviceCreateInfo{};
            deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

            //TODO: Add other queues (Compute, Transfer).
            deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
            // The number of queue create infos is guaranteed to fit within uint32_t by Vulkan API constraints.
            deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());

            deviceCreateInfo.pEnabledFeatures = &deviceFeatures;

            deviceCreateInfo.ppEnabledExtensionNames = requiredExt.data();
            deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExt.size());
            
#ifdef NDEBUG
            deviceCreateInfo.enabledLayerCount = 0;
#else
            deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            deviceCreateInfo.ppEnabledLayerNames = validationLayers.data();
#endif
            if(vkCreateDevice(bestDev.vkPhysicalDevice, &deviceCreateInfo, nullptr, &device) != VK_SUCCESS){
                Logger::Log("(vk) Unable to create device.");
                return ErrorCode::FAILED;
            }
            

            vkGetDeviceQueue(device, bestDev.Graphics.Index.value(), 0, &vkRenderer::GraphicsQueue.queue);
            vkRenderer::GraphicsQueue.index = bestDev.Graphics.Index.value();
            if(bestDev.Graphics.Index == bestDev.Present.Index){
                vkRenderer::PresentQueue = vkRenderer::GraphicsQueue;
                vkRenderer::PresentQueue.index = bestDev.Graphics.Index.value();
            }else{
                vkGetDeviceQueue(device, bestDev.Present.Index.value(), 0, &vkRenderer::PresentQueue.queue);
                vkRenderer::PresentQueue.index = bestDev.Present.Index.value();
            }



            return ErrorCode::OK;
        }
        /**
         * @brief Queries the available devices and scores them based on their features, then caches the Query.
         * This function is called if a device info cache is not found.
         */
        static std::multimap<int, RuntimeDeviceData> QueryDevices(VkSurfaceKHR surface) noexcept{
            uint32_t deviceCount = 0;
            vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

            if(deviceCount == 0){
                Logger::Log("Failed to find GPUs with Vulkan support. Verify that your driver supports Vulkan");
                return std::multimap<int, RuntimeDeviceData>();
            }

            std::vector<VkPhysicalDevice> devices(deviceCount);
            vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
            std::multimap<int, RuntimeDeviceData> deviceMap;

            std::fstream file;

            if(std::filesystem::exists("GPUDevice.cache")){
                Logger::Log("GPU Device info cache found.");
                //TODO: read the file.
                file = std::fstream("GPUDevice.cache", std::ios::out | std::ios::binary);
                if(file.is_open()){
                    file.write((char*)&deviceCount, sizeof(size_t));
                }
            }else{
                Logger::Log("GPU Device info cache not found.");
                file = std::fstream("GPUDevice.cache", std::ios::out | std::ios::binary);
                if(file.is_open()){
                    file.write((char*)&deviceCount, sizeof(size_t));
                }
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

                if(!d.ExtSupported){ continue; } //Ignore Devices that don't support needed extension (e.g: Swapchain)
                //Queue Family Scoring.
                uint32_t queueFamilyCount = 0;
                vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

                std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
                vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

                QueueFamily GraphicQueueFamily;
                //Dedicated Compute Queue family
                QueueFamily ComputeQueueFamily;
                //Having every dedicated Transfer queue family is never a problem i think.
                std::vector<QueueFamily> TransferQueueFamily;
                //Used in case the Graphics Queue Family Doesn't support Surface Present.
                //This will hold the first Queue Family that supports Surface Presenting.
                QueueFamily PresentQueueFamily;


                unsigned int queueIndex = 0;
                uint32_t SurfaceSupport = false;
                for(const auto& queueFamily : queueFamilies){
                    SurfaceSupport = false;
                    vkGetPhysicalDeviceSurfaceSupportKHR(device, queueIndex, surface, &SurfaceSupport);

                    if(!PresentQueueFamily.Index.has_value()){
                        d.Score += 1000;//Very Important.
                        PresentQueueFamily.Index = queueIndex;
                        PresentQueueFamily.Count = queueFamily.queueCount;
                        PresentQueueFamily.PresentSupport = true;
                    }

                    Logger::Log("Device Queue Families {} Flags: {}{}{}{} Surface Support: {}", queueIndex,
                        queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT ? "Graphics " : "",
                        queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT ? "Compute " : "",
                        queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT ? "Transfer " : "",
                        queueFamily.queueFlags & VK_QUEUE_SPARSE_BINDING_BIT ? "Sparce " : "",
                        SurfaceSupport ? "True" : "False");
                    Logger::Log("Queue Count: {}", queueFamily.queueCount);
                    if(queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT){
                        d.Score += 100;//Graphics is a general purpose feature.
                        if(!GraphicQueueFamily.Index.has_value()){
                            GraphicQueueFamily.Count = queueFamily.queueCount;
                            GraphicQueueFamily.Index = queueIndex;
                            GraphicQueueFamily.PresentSupport = SurfaceSupport;
                        }else if(GraphicQueueFamily.Count < queueFamily.queueCount){
                            GraphicQueueFamily.Count = queueFamily.queueCount;
                            GraphicQueueFamily.Index = queueIndex;
                            GraphicQueueFamily.PresentSupport = SurfaceSupport;
                        }else if(!(queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT) && !(queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT)){
                            //Favor dedicated graphics queue if found.
                            GraphicQueueFamily.Count = queueFamily.queueCount;
                            GraphicQueueFamily.Index = queueIndex;
                            GraphicQueueFamily.PresentSupport = SurfaceSupport;
                            continue;
                        }
                        //Check for Surface support.
                    }
                    if((queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT) && !(queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)){
                        d.Score += 150;//dedicated Compute queue is a beneficial feature.
                        if(!ComputeQueueFamily.Index.has_value()){
                            ComputeQueueFamily.Index = queueIndex;
                            ComputeQueueFamily.Count = queueFamily.queueCount;
                            ComputeQueueFamily.PresentSupport = SurfaceSupport;
                        }
                        continue;
                    }
                    if(queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT){
                        if(!(queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
                        && !(queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT)){
                            d.Score += 150;//Dedicated transfer queue.
                            QueueFamily qf{ queueIndex, queueFamily.queueCount, SurfaceSupport};
                            TransferQueueFamily.push_back(qf);
                        }
                        continue;
                    }

                    ++queueIndex;
                }//Queue Family Scoring

                RuntimeDeviceData rt_dev = RuntimeDeviceData();
                rt_dev.device = d;
                rt_dev.Graphics = GraphicQueueFamily;
                rt_dev.Compute = ComputeQueueFamily;
                rt_dev.Transfer = TransferQueueFamily;
                rt_dev.Present = PresentQueueFamily;
                rt_dev.vkPhysicalDevice = device;

                deviceMap.insert(std::make_pair(d.Score, rt_dev));
                Logger::Log("Device Score: {}", d.Score);
                //Cache the scored devices.
                if(file.is_open()){
                    file.write((char*)&d, sizeof(Device));
                    uint32_t index;
                    bool hasvalue;
                    hasvalue = GraphicQueueFamily.Index.has_value();
                    file.write(reinterpret_cast<char*>(&hasvalue), sizeof(bool));
                    if(hasvalue){
                        index = reinterpret_cast<uint32_t&&>(GraphicQueueFamily.Index.value());
                        file.write((char*)&index, sizeof(uint32_t));
                        file.write((char*)&GraphicQueueFamily.Count, sizeof(uint32_t));
                    }
                    hasvalue = ComputeQueueFamily.Index.has_value();
                    file.write((char*)&hasvalue, sizeof(bool));
                    if(hasvalue){
                        index = reinterpret_cast<uint32_t&&>(ComputeQueueFamily.Index.value());
                        file.write((char*)&index, sizeof(uint32_t));
                        file.write((char*)&ComputeQueueFamily.Count, sizeof(uint32_t));
                    }
                    size_t count = TransferQueueFamily.size();
                    file.write((char*)&count, sizeof(size_t));
                    for(size_t i = 0; i < count; i++){
                        hasvalue = TransferQueueFamily[i].Index.has_value();
                        file.write((char*)&hasvalue, sizeof(bool));
                        if(hasvalue){
                            index = reinterpret_cast<uint32_t&&>(TransferQueueFamily[i].Index.value());
                            file.write((char*)&index, sizeof(uint32_t));
                            file.write((char*)&TransferQueueFamily[i].Count, sizeof(uint32_t));
                        }
                    }
                    hasvalue = PresentQueueFamily.Index.has_value();
                    file.write((char*)&hasvalue, sizeof(bool));
                    if(hasvalue){
                        index = reinterpret_cast<uint32_t&&>(PresentQueueFamily.Index.value());
                        file.write((char*)&index, sizeof(uint32_t));
                        file.write((char*)&PresentQueueFamily.Count, sizeof(uint32_t));
                    }
                }
            }//Device Scoring
            file.close();
            return deviceMap;
        }

        static void Cleanup(){
            vkDestroyDevice(device, nullptr);
            vkDestroyInstance(instance, nullptr);

            if (enableValidationLayers) {
                DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
            }
        }

        static const VkAllocationCallbacks* GetAllocator() noexcept {
            return nullptr;
        }

        static void SetAllocator(VkAllocationCallbacks cb) /*noexcept*/ {
            throw std::exception("Not Implemented");
        }

        static void setupDebugMessenger() {
            if (!enableValidationLayers) return;
    
            VkDebugUtilsMessengerCreateInfoEXT createInfo;
            populateDebugMessengerCreateInfo(createInfo);
    
            if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
                throw std::runtime_error("failed to set up debug messenger!");
            }
        }

        static void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
            createInfo = {};
            createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
            createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
            createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
            createInfo.pfnUserCallback = debugCallback;
        }

        static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
            std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
    
            return VK_FALSE;
        }
    };

}