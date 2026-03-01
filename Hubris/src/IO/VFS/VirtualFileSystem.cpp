#include "IO/VFS/VirtualFileSystem.h"
#include <algorithm>

namespace Hubris::IO::VFS {

    void VirtualFileSystem::Mount(Handle<IAssetLoader> loader, int priority) {
        std::lock_guard lock(m_mutex);
        m_loaders.push_back({ std::move(loader), priority });
        SortLoadersByPriority();
    }

    void VirtualFileSystem::Unmount(std::string_view loaderName) {
        std::lock_guard lock(m_mutex);
        auto it = std::remove_if(m_loaders.begin(), m_loaders.end(),
            [loaderName](const LoaderEntry& entry) {
                return entry.loader->Name() == loaderName;
            });
        m_loaders.erase(it, m_loaders.end());
    }

    std::optional<AssetHandle> VirtualFileSystem::Load(std::string_view virtualPath) {
        std::lock_guard lock(m_mutex);

        // Check cache first
        uint64_t pathHash = std::hash<std::string_view>{}(virtualPath);
        if (auto it = m_cache.find(pathHash); it != m_cache.end()) {
            return it->second;
        }

        // Try each loader in priority order
        for (size_t i = 0; i < m_loaders.size(); ++i) {
            auto& entry = m_loaders[i];
            if (entry.loader->CanHandle(virtualPath)) {
                if (auto handle = entry.loader->Load(virtualPath)) {
                    handle->id = pathHash;
                    m_handleToLoader[pathHash] = i;
                    m_cache[pathHash] = *handle;
                    return handle;
                }
            }
        }

        return std::nullopt;
    }

    void VirtualFileSystem::Unload(AssetHandle& handle) {
        std::lock_guard lock(m_mutex);

        if (auto it = m_handleToLoader.find(handle.id); it != m_handleToLoader.end()) {
            m_loaders[it->second].loader->Unload(handle);
            m_handleToLoader.erase(it);
            m_cache.erase(handle.id);
        }

        handle = {}; // Reset handle
    }

    bool VirtualFileSystem::Exists(std::string_view virtualPath) const {
        std::lock_guard lock(m_mutex);

        uint64_t pathHash = std::hash<std::string_view>{}(virtualPath);
        if (m_cache.contains(pathHash)) {
            return true;
        }

        for (const auto& entry : m_loaders) {
            if (entry.loader->CanHandle(virtualPath)) {
                return true; // Assume it exists if loader can handle it
            }
        }
        return false;
    }

    std::vector<std::string> VirtualFileSystem::GetMountedLoaders() const {
        std::lock_guard lock(m_mutex);
        std::vector<std::string> names;
        names.reserve(m_loaders.size());
        for (const auto& entry : m_loaders) {
            names.emplace_back(entry.loader->Name());
        }
        return names;
    }

    void VirtualFileSystem::ClearCache() {
        std::lock_guard lock(m_mutex);
        
        // Unload all cached assets
        for (auto& [id, handle] : m_cache) {
            if (auto it = m_handleToLoader.find(id); it != m_handleToLoader.end()) {
                m_loaders[it->second].loader->Unload(handle);
            }
        }
        
        m_cache.clear();
        m_handleToLoader.clear();
    }

    void VirtualFileSystem::SortLoadersByPriority() {
        std::sort(m_loaders.begin(), m_loaders.end(),
            [](const LoaderEntry& a, const LoaderEntry& b) {
                return a.priority > b.priority; // Higher priority first
            });
    }

} // namespace Hubris::IO::VFS
