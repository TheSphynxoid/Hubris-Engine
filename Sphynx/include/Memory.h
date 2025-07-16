#pragma once
#include <memory>
#include <version>
//What ? Did I copy this from the old engine ?

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

    //Unique pointer equiv. Handle owns the object referenced.
    template<typename T>
    struct Handle{
    private:
        T* raw = nullptr;
    public:
        constexpr Handle() noexcept = default;
        constexpr Handle(T* t) noexcept {
            raw = t;
        }

        template<typename U, typename = std::enable_if_t<std::is_convertible_v<U*, T*> && !std::is_same_v<std::remove_cv_t<U>, std::remove_cv_t<T>>>>
        constexpr explicit Handle(U* other) noexcept {
            static_assert(
                !std::is_polymorphic_v<U> || (std::has_virtual_destructor_v<T> && std::is_base_of_v<T, U>),
                "If U is polymorphic, T must be a polymorphic base with a virtual destructor"
            );//If U is polymorphic => U*->T* => T*.~T() must be virtual <=> T base of U

            //This is safe even without the cast as the SFINAE ensures only convertible types are allowed,
            //This is done work for explicit cast operators.
            raw = static_cast<T*>(other);
        }
        
        template<typename U, typename = std::enable_if_t<std::is_convertible_v<U*, T*> && !std::is_same_v<U, T>>>
        constexpr Handle(Handle<U>&& other) noexcept {
            reset();
            std::swap(this->raw, other.raw);
        }

        constexpr Handle(Handle&& mv) noexcept {
            std::swap(this->raw, mv.raw);
        }

        ~Handle() noexcept {
            reset();
        }        
        Handle& operator=(Handle&& mv) noexcept{
            if(this == &mv)return *this;
            reset();
            std::swap(this->raw, mv.raw);
            return *this;
        }
        Handle(const Handle& cp) noexcept = delete;
        Handle& operator=(const Handle& cp) = delete;

        constexpr T* get() noexcept {return raw;}
        constexpr const T* get()const noexcept {return raw;}

        inline void reset(T* ptr = nullptr) noexcept{
            if(raw != ptr){
                delete raw; //delete nullptr is both safe and defined as no-op.
                raw = ptr;
            }
        }

        constexpr T* release() noexcept /*dangerous_release*/ {
            return std::exchange(raw, nullptr);
        }
        
        template<typename U, typename = std::enable_if_t<std::is_convertible_v<U*, T*>>>
        constexpr void swap(Handle<U>& rhs) noexcept{
            static_assert(
                !std::is_polymorphic_v<U> || (std::has_virtual_destructor_v<T> && std::is_base_of_v<T, U>),
                "If U is polymorphic, T must be a polymorphic base with a virtual destructor"
            );
            std::swap(this->raw, rhs.raw);
        }

        constexpr T* operator->() noexcept{
            return raw;
        }
        constexpr const T* operator->()const noexcept{
            return raw;
        }
        constexpr T& operator*() { return *raw;} //this can throw, it's the users responsibility to check for ptr validity.
        constexpr const T& operator*() const {return *raw;} //this can throw, it's the users responsibility to check for ptr validity.
        
        template<typename U, typename = std::enable_if_t<std::is_convertible_v<U*, T*>>>
        constexpr bool operator==(const Handle<U>& rhs)const noexcept{
            return rhs.raw == this->raw;
        }

        template<typename U, typename = std::enable_if_t<std::is_convertible_v<U*, T*>>>
        constexpr bool operator!=(const Handle<U>& rhs)const noexcept{
            return rhs.raw != this->raw;
        }
        //This is fine and left for simple checks.
        constexpr operator bool()const noexcept{
            return this->raw;
        }

        constexpr bool operator!() const noexcept { return this->raw; }
        
        //Explicitly disallow cast operators
        operator T*() = delete;
        operator void*() = delete;
    };
}

template<typename T>
constexpr void swap(Sphynx::Handle<T>& lhs, Sphynx::Handle<T>& rhs) noexcept {
    lhs.swap(rhs);
};

template<typename T, typename U, typename = std::enable_if_t<std::is_convertible_v<U*, T*> && !std::is_same_v<U, T>>>
constexpr void swap(Sphynx::Handle<T>& lhs, Sphynx::Handle<U>& rhs) noexcept {
    static_assert(
        !std::is_polymorphic_v<U> || (std::has_virtual_destructor_v<T> && std::is_base_of_v<T, U>),
        "If U is polymorphic, T must be a polymorphic base with a virtual destructor"
    );
    lhs.swap(rhs);
}