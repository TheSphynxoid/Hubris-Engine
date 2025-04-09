#pragma once
//TODO: Why the fuck is there two memory.h files ?

namespace Sphynx{
    struct Block{
        size_t blk_id;
        void* pointer;

        // constexpr operator void*()const noexcept{
        //     return pointer;
        // }
    };

    /**
     * @brief Shared blocks are thread shared blocks.
     */
    struct SharedBlock{

    };
    
    struct Arena{
        size_t Size;
        
    };
    class Memory{
    private:

    public:
        /**
         * @brief Allocates a thread-local block.
         */
        static Block Alloc(size_t bufSize);
        /**
         * @brief Attempts to resize a block, this can move the block to a diffrent location.
         * 
         * If this fails, the original data is unchanged.
         * 
         * If the block is moved, the data is copied and the old block is freed.
         * 
         * @param block the block to be resized.
         * @param newSize the new size.
         * @return 
         */
        static Block Resize(Block& block, size_t newSize);
        /**
         * @brief Frees a buffer.
         */
        static void Free(Block& buffer);
        /**
         * @brief Internal use. Arenas are not expandable except the internal arena.
         * 
         *
         * @param width new width.
         */
        static void ExpandInternalArena(size_t width);
        /**
         * @brief Creates a thread-local arena.
         * 
         *
         * @param size Arena size.
         * @return Reference to the new arena.
         */
        static Arena& CreateArena(size_t size);
        /**
         * @brief Allocates a block with syncing constructs.
         * 
         */
        static SharedBlock AllocShared(size_t size);
        /**
         * @brief Allocates a shared arena for multi-threaded use.
         *
         * @param width Arena size.
         * @return A reference to the new arena.
         */
        static Arena& CreateGlobalArena(size_t width);
        /**
         * @brief Attempts to free an areana, if the arean has any blocks in use this call with throw.
         */
        static void FreeArena(Arena);
        //Utility Checks.
        static bool IsValid(const void* pointer);
        static bool IsEngineAllocated(const void* pointer);
        static bool IsFree(const Block& block)noexcept;
        static bool IsThreadLocal(const Block& block)noexcept;
        static bool IsShared(const void* blockptr)noexcept;
        //Statistics
        static size_t MemoryUsed(const Arena& arena);
        static size_t GetMemoryAvailable(const Arena& arena);
        static unsigned int TotalAllocationCount()noexcept;
        static unsigned int TotalDeallocationCount()noexcept;
        static unsigned int TotalBlockResizeCount()noexcept;
        static unsigned int TotalArenasAllocated()noexcept;
        static unsigned int TotalArenasFreed()noexcept;
        static unsigned int ThreadAllocationCount()noexcept;
        static unsigned int ThreadDeallocationCount()noexcept;
        static unsigned int ThreadBlockResizeCount()noexcept;
        static unsigned int ThreadArenasAllocated()noexcept;
        static unsigned int ThreadArenasFreed()noexcept;
        static size_t TotalMemoryAllocated()noexcept;
        static size_t TotalMemoryFreed()noexcept;
        static size_t ThreadMemoryUsed()noexcept;
        static size_t ThreadMemoryFreed()noexcept;
    };
}