#include "IO/VFS/FileSystemLoader.h"
#include <fstream>

namespace Hubris::IO::VFS {

    FileSystemLoader::FileSystemLoader(
        std::filesystem::path rootPath,
        std::string mountPoint,
        int priority
    ) : m_rootPath(std::move(rootPath)),
        m_mountPoint(std::move(mountPoint)),
        m_priority(priority)
    {
        // Ensure mount point ends with ://
        if (!m_mountPoint.ends_with("://")) {
            if (m_mountPoint.ends_with(":")) {
                m_mountPoint += "//";
            } else {
                m_mountPoint += "://";
            }
        }
    }

    std::optional<AssetHandle> FileSystemLoader::Load(std::string_view virtualPath) {
        auto physicalPath = ResolvePhysicalPath(virtualPath);

        if (!std::filesystem::exists(physicalPath)) {
            return std::nullopt;
        }

        // Read file into memory
        std::ifstream file(physicalPath, std::ios::binary | std::ios::ate);
        if (!file.is_open()) {
            return std::nullopt;
        }

        size_t fileSize = static_cast<size_t>(file.tellg());
        file.seekg(0);

        auto* buffer = new uint8_t[fileSize];
        file.read(reinterpret_cast<char*>(buffer), fileSize);
        file.close();

        AssetHandle handle;
        handle.type = DeduceAssetType(physicalPath);
        handle.data = std::span<const uint8_t>(buffer, fileSize);
        handle.virtualPath = std::string(virtualPath);
        handle.userData = buffer; // Store for cleanup

        return handle;
    }

    void FileSystemLoader::Unload(AssetHandle& handle) {
        if (handle.userData) {
            delete[] static_cast<uint8_t*>(handle.userData);
            handle.userData = nullptr;
        }
        handle.data = {};
    }

    bool FileSystemLoader::CanHandle(std::string_view virtualPath) const {
        return virtualPath.starts_with(m_mountPoint);
    }

    std::filesystem::path FileSystemLoader::ResolvePhysicalPath(std::string_view virtualPath) const {
        // Strip mount point prefix
        auto relativePath = virtualPath.substr(m_mountPoint.length());
        return m_rootPath / relativePath;
    }

    AssetType FileSystemLoader::DeduceAssetType(const std::filesystem::path& path) const {
        auto ext = path.extension().string();
        
        if (ext == ".spv" || ext == ".spirv" || ext == ".vert" || ext == ".frag" || ext == ".comp") {
            return AssetType::Shader;
        }
        if (ext == ".png" || ext == ".jpg" || ext == ".jpeg" || ext == ".dds" || ext == ".ktx") {
            return AssetType::Texture;
        }
        if (ext == ".obj" || ext == ".fbx" || ext == ".gltf" || ext == ".glb") {
            return AssetType::Model;
        }
        if (ext == ".wav" || ext == ".mp3" || ext == ".ogg") {
            return AssetType::Audio;
        }
        if (ext == ".json" || ext == ".yaml" || ext == ".xml") {
            return AssetType::Config;
        }

        return AssetType::Unknown;
    }

} // namespace Hubris::IO::VFS
