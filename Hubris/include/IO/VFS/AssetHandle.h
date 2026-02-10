#pragma once
#include <cstdint>
#include <string>
#include <span>

namespace Hubris::IO::VFS {

    /**
     * @brief Asset type enumeration for runtime type checking
     */
    enum class AssetType : uint16_t {
        Unknown = 0,
        Shader,
        Texture,
        Model,
        Material,
        Audio,
        Config,
        // Add more as needed
    };

    /**
     * @brief Asset handle returned by VFS loaders
     * Represents a loaded asset in memory with metadata
     */
    struct AssetHandle {
        uint64_t id = 0;                    ///< Unique asset identifier (hash of path)
        AssetType type = AssetType::Unknown; ///< Type of asset
        std::span<const uint8_t> data;      ///< Read-only view of asset data
        std::string virtualPath;             ///< Virtual path used to load (e.g., "shaders://vertex.spv")
        void* userData = nullptr;            ///< Loader-specific data (for cleanup)
        
        /**
         * @brief Check if handle is valid
         */
        [[nodiscard]] bool IsValid() const noexcept {
            return id != 0 && !data.empty();
        }

        /**
         * @brief Get size in bytes
         */
        [[nodiscard]] size_t Size() const noexcept {
            return data.size();
        }

        /**
         * @brief Get raw pointer (for legacy APIs)
         */
        [[nodiscard]] const void* Raw() const noexcept {
            return data.data();
        }
    };

} // namespace Hubris::IO::VFS
