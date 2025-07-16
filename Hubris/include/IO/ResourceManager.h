#pragma once
#include <filesystem>


namespace Hubris::IO {
    class IOBuffer {
    private:
        const void* buffer = nullptr;
        size_t buffersize = 0;
        
    public:
        IOBuffer()noexcept = default;
        IOBuffer(const void* buffer, const size_t buffersize)noexcept {
            this->buffer = buffer;
            this->buffersize = buffersize;
        }
        ~IOBuffer() {
            delete buffer;
            buffersize = 0;
            buffer = nullptr;
        }
        inline const size_t& size()const noexcept { return buffersize; };
        inline const void* get_raw()const noexcept { return buffer; };
    };

    class ExtensionHandler {
    public:
        virtual ~ExtensionHandler() = 0;

        virtual IOBuffer& Read(const std::string& Path, bool RelativePath = true) = 0;
        virtual void Write(const void* Buffer, const size_t Buf_Size) = 0;
    };
    struct Extension {
        /**
         * @brief Use | to seperate between handled file extensions.
         */
        const char* extPostfix;
        ExtensionHandler* Handler;
    };


    class Asset {

    };


    template<typename T>
    concept AssetHandlerConcept = requires(const std::string & path, std::shared_ptr<Asset> asset) {

        typename T::AssetType;
        // Must have Load(path)
        { T::Load(path) } -> std::same_as<std::shared_ptr<typename T::AssetType>>;

        // Must have Save(asset, path)
        { T::Save(asset, path) } -> std::same_as<void>;

        // Must have Import(path)
        { T::Import(path) } -> std::same_as<std::shared_ptr<typename T::AssetType>>;

        // Must have GetHandledType()
        { T::GetHandledType() } -> std::same_as<std::string>;
    };

    class ResourceManager {
    private:
        typedef std::unordered_map<std::string, ExtensionHandler*> ExtensionHandlerMap;
        static inline ExtensionHandlerMap HandlerMap = ExtensionHandlerMap(25);


        static std::vector<std::string> ParseExtensions(const std::string& extPostfix) {
            std::stringstream ss(extPostfix);
            std::string extension;
            std::vector<std::string> extensions;

            while (std::getline(ss, extension, '|')) {
                extension.erase(0, extension.find_first_not_of(" \t\n\r\f\v"));
                extension.erase(extension.find_last_not_of(" \t\n\r\f\v") + 1);
                extensions.push_back(extension);
            }
            return extensions;
        }
    public:
        static void SetExtensionHandler(const Extension& ext) {
            //Parse The postfix. | is used to seperate between file extension handled.
            std::stringstream ss(ext.extPostfix);
            std::string extension;

            while (std::getline(ss, extension, '|')) {
                // Remove any leading or trailing whitespace
                extension.erase(0, extension.find_first_not_of(" \t\n\r\f\v"));
                extension.erase(extension.find_last_not_of(" \t\n\r\f\v") + 1);

                // Add the extension and its handler to the map
                HandlerMap[extension] = ext.Handler;
            }
        }

        static IOBuffer& ReadFile(const std::string& path) {
            size_t dotPos = path.find_last_of('.');
            if (dotPos == std::string::npos) {
                // No extension found
                static IOBuffer NULLBUF(nullptr, 0);
                return NULLBUF;
            }
            auto& handler = HandlerMap[path.substr(dotPos + 1)];

            return handler->Read(path);
        }

        
        
    };
    /**
     * @brief Temporary function until asset and resource management is implemented.
     * 
     * @param filename 
     * @return std::vector<char> 
     */
    static std::vector<char> readFile(const std::string& filename) {
        std::ifstream file(filename, std::ios::ate | std::ios::binary);
    
        if (!file.is_open()) {
            throw std::runtime_error("failed to open file!");
        }

        size_t fileSize = (size_t) file.tellg();
        std::vector<char> buffer(fileSize);
        
        file.seekg(0);  
        file.read(buffer.data(), fileSize);

        file.close();
        return buffer;
    }

}