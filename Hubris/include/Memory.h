#pragma once
#include <memory>
#include <version>
#include <cstddef>
#include <new>
#include <tuple>
#include <utility>
#include <type_traits>
#include <algorithm>
#include <cassert>

namespace Hubris{
    template<typename From, typename To>
    concept Castable = std::convertible_to<From, To> && !std::same_as<From, To>;
    template<typename From, typename To>
    concept PolymorphicConvertible = std::convertible_to<From*, To*> && std::derived_from<From, To> && std::has_virtual_destructor_v<To>;
    template<typename From, typename To>
    concept SafelyUpcastable = Castable<From*, To*> && std::derived_from<From, To>&& std::has_virtual_destructor_v<To>;
    template<typename T, typename V>
    concept SameUnqualifiedType = std::same_as<std::remove_cv_t<T>, std::remove_cv_t<V>>;
    template<typename T, typename V>
    concept CompatibleHandle = SameUnqualifiedType<T, V>&& SafelyUpcastable<T, V>;

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

    
    template<typename T>
    struct remove_all_pointers{
        using type = T;
    };

    template<typename T>
    struct remove_all_pointers<T*> {
        using type = typename remove_all_pointers<T>::type;
    };
    template<typename T>
    struct remove_all_pointers<T* const> {
        using type = typename remove_all_pointers<T>::type;
    };

    template<typename T>
    struct remove_all_pointers<T* volatile> {
        using type = typename remove_all_pointers<T>::type;
    };

    template<typename T>
    struct remove_all_pointers<T* const volatile> {
        using type = typename remove_all_pointers<T>::type;
    };

    template<typename T>
    using remove_all_pointers_t = typename remove_all_pointers<T>::type;


    template<typename T>
    concept IsType = !std::is_void_v<remove_all_pointers_t<T>> && !std::is_null_pointer_v<T>;

    template<typename T> requires IsType<T>
    struct Weak;
    template<typename T> requires IsType<T>
    struct Handle;

    struct ControlBlock{
        void* raw;
        std::atomic_uint32_t ref_count = { 1 };
        std::atomic_uint32_t weak_count = { 1 };  // Implicit weak ref held by strong refs
        void* BaseLocation;
        size_t alloc_align;
        bool co_allocated = true;  // true if allocated via CoAllocate, false if manually created
    };

    template<typename T>
    consteval size_t padded_size() {
        return (sizeof(T) + alignof(T) - 1) & ~(alignof(T) - 1);
    }

    
    template<typename T, typename F>
    void Traverse(T&& container, F&& for_each_leaf) {
        if constexpr (std::ranges::range<T> && !std::is_same_v<std::decay_t<T>, std::string>) {
            for (auto&& elem : container) {
                Traverse(std::forward<decltype(elem)>(elem), for_each_leaf);
            }
        } else {
            for_each_leaf(std::forward<T>(container));
        }
    }

    
    /// @brief Allocates memory for the ControlBlock and T but does not construct T. This can lead to UB if not used carefully.
    /// @return a controlblock pointing at the unintialized T
    template<typename T>
    constexpr ControlBlock* CoAllocate_Unsafe(){
        static_assert(!std::is_unbounded_array_v<T>, "Cannot CoAllocate Unbounded arrays (at least for now, may create a special handle)");
        constexpr size_t alloc_align = std::max(alignof(ControlBlock), alignof(T));
        constexpr size_t Toffset =  alignof(ControlBlock) > alignof(T) ? padded_size<ControlBlock>() : 0;
        constexpr size_t CTR_offset = alignof(ControlBlock) > alignof(T) ? 0 : padded_size<ControlBlock>();
        char* ptr = (char*)(alignof(ControlBlock) > alignof(T)
            ? operator new(padded_size<ControlBlock>() + sizeof(T), std::align_val_t{ alloc_align }, std::nothrow)
            : operator new(padded_size<T>() + sizeof(ControlBlock), std::align_val_t{ alloc_align }, std::nothrow));
        
        if (!ptr) return nullptr;
        
        //This is aligned because either it is at the start (alignof(Ctr) > alignof(T)) or T is padded at the end to align make this aligned
        ControlBlock* ctr = new(ptr + CTR_offset)ControlBlock();
        if(!ctr)return nullptr;
        ctr->alloc_align = alloc_align;
        ctr->BaseLocation = ptr;
        //The logic is flipped but applies here.
        ctr->raw = ptr + Toffset;

        return ctr;
    }

    /// @brief Constructs any object at location (placment new) with the specified arguments.
    /// @tparam T Type to be constructed
    /// @tparam ...Args Type of arguments for the .Ctor.
    /// @param placement Location of memory to place T in.
    /// @param ...args value to be forwarded to the .Ctor.
    template<typename T, typename ...Args>
    constexpr void Construct_T_Inplace(void* placement, Args&& ...args){
        // static_assert(std::is_pointer<T>)
        new(placement)std::remove_cvref_t<T>(std::forward<Args>(args)...);
    }

    /// @brief Allocates and constructs both the ControlBlock and the Object T and returns the instanciated controlblock
    /// @tparam ...Args Argument Types for the Constructor of T
    /// @param ...args  Argument values to be passed to the constructor of T::T(Args&& ...args)
    /// @return a fully intialized controlblock pointing at the newly constructed T
    template<typename T, typename ...Args>
    constexpr ControlBlock* CoAllocate(Args&& ...args){
        ControlBlock* ctr = CoAllocate_Unsafe<T>();
        if(!ctr)return nullptr;
        Construct_T_Inplace<T, Args...>(ctr->raw, std::forward<Args>(args)...);
        return ctr;
    }

    // template<typename T>
    // constexpr void CoDelete(ControlBlock* ctr){
    //     assert(ctr->raw && "ControlBlock is observing no object");
    //     if constexpr(!std::is_array_v<T>){

    //     }
    // }
    
    template<typename T> requires IsType<T>
    struct Shared {
        static_assert(!std::is_reference_v<T>, "No reference is allowed.");
        static_assert(!std::is_array_v<T>, "Use Shared<T[N]> for bounded arrays; unbounded arrays are not supported.");
        using Unqualified = std::remove_cv_t<T>;
        
        // Allow all Shared<U> instantiations to access private members
        template<typename U> requires IsType<U>
        friend struct Shared;
        template<typename U> requires IsType<U>
        friend struct Weak;
        
        #ifdef _DEBUG
        bool HasBeenConverted = false;
        const type_info* Converted_Type_Info = &typeid(T);
        #endif
    private:
        /// @brief The control block must never be assigned by the user, 
        /// it must be verified that the underlying types before assigning as it is abstracted through a void* to allow for conversion (only polymorphic conversion is allowed).
        ControlBlock* ctr_blk = nullptr;

        /// @brief Create a Shared pointer wrapped around a pointer allocated with scalar ::operator new.
        /// @warning The pointer MUST have been allocated with `new T(...)` (not new[], not placement new).
        /// @param t Pointer to T. Ownership is transferred.
        constexpr Shared(T* t) noexcept {
            ctr_blk = new ControlBlock;
            ctr_blk->raw = (void*)t;
            ctr_blk->co_allocated = false;  // Manually created control block
        }

    public:
        constexpr Shared() noexcept = default;

        // Same-type copy constructor
        constexpr Shared(const Shared& other) noexcept : ctr_blk(other.ctr_blk) {
            if (ctr_blk) {
                ctr_blk->ref_count.fetch_add(1, std::memory_order_relaxed);
            }
        }

        // Same-type move constructor
        constexpr Shared(Shared&& other) noexcept : ctr_blk(std::exchange(other.ctr_blk, nullptr)) {}

        // Same-type copy assignment
        Shared& operator=(const Shared& other) noexcept {
            if (this->ctr_blk == other.ctr_blk) return *this;
            Release();
            this->ctr_blk = other.ctr_blk;
            if (ctr_blk) {
                ctr_blk->ref_count.fetch_add(1, std::memory_order_relaxed);
            }
            return *this;
        }

        // Same-type move assignment
        Shared& operator=(Shared&& other) noexcept {
            if (this->ctr_blk == other.ctr_blk) return *this;
            Release();
            ctr_blk = std::exchange(other.ctr_blk, nullptr);
            return *this;
        }

        template<typename ...Args>
            requires std::constructible_from<T, Args...> && 
                     (sizeof...(Args) != 1 || 
                      (!std::is_same_v<std::remove_cvref_t<Args>, Shared> && ...) &&
                      (!std::is_same_v<std::remove_cvref_t<Args>, Weak<T>> && ...))
        constexpr Shared(Args&& ...args){
            //For now, CoAllocates blocks unbounded arrays. So calling dtor is possible by indexing the array.
            ctr_blk = CoAllocate<std::remove_cv_t<T>>(std::forward<Args>(args)...);
        }

        constexpr explicit Shared(Weak<T>&& promote) noexcept {
            if (!promote.ctr_blk) return;
            uint32_t count = promote.ctr_blk->ref_count.load(std::memory_order_relaxed);
            do {
                if (count == 0) return;  // expired — leave this Shared empty
            } while (!promote.ctr_blk->ref_count.compare_exchange_weak(
                count, count + 1, std::memory_order_acq_rel, std::memory_order_relaxed));
            
            // Successfully atomically incremented ref_count — safe to take ownership
            this->ctr_blk = promote.ctr_blk;
            promote.Reset();
        }

        template<typename U>
        constexpr explicit Shared(const Shared<U>& other) noexcept requires PolymorphicConvertible<U, T> {
            this->ctr_blk = other.ctr_blk;
            if (this->ctr_blk) {
                this->ctr_blk->ref_count.fetch_add(1, std::memory_order_relaxed);
            }
            #ifdef _DEBUG
            if constexpr (!std::is_same_v<std::remove_cv_t<T>, std::remove_cv_t<U>>){
                HasBeenConverted = true;
                Converted_Type_Info = &typeid(U);
            }
            #endif
        }
                
        template<typename U>
        Shared& operator=(const Shared<U>& cpy) noexcept requires PolymorphicConvertible<U, T>{
            if(this->ctr_blk == cpy.ctr_blk) return *this;
            Release();
            this->ctr_blk = cpy.ctr_blk;
            //Copied an "empty" Shared into this one. User at fault but still we do what they wanted.
            if(!ctr_blk) return *this;
            ctr_blk->ref_count.fetch_add(1, std::memory_order_relaxed);
            #ifdef _DEBUG
            if constexpr (!std::is_same_v<std::remove_cv_t<T>, std::remove_cv_t<U>>){
                HasBeenConverted = true;
                Converted_Type_Info = &typeid(U);
            }
            #endif
            return *this;
        }

        template<typename U>
        constexpr explicit Shared(Shared<U>&& other) noexcept requires PolymorphicConvertible<U, T> {
            this->ctr_blk = std::exchange(other.ctr_blk, nullptr);
            #ifdef _DEBUG
            if constexpr (!std::is_same_v<std::remove_cv_t<T>, std::remove_cv_t<U>>){
                HasBeenConverted = true;
                Converted_Type_Info = &typeid(U);
            }
            #endif
        }
        
        template<typename U>
        Shared& operator=(Shared<U>&& mv) noexcept requires PolymorphicConvertible<U, T> {
            if(this->ctr_blk == mv.ctr_blk) return *this;
            Release();
            this->ctr_blk = std::exchange(mv.ctr_blk, nullptr);
            #ifdef _DEBUG
            if constexpr (!std::is_same_v<std::remove_cv_t<T>, std::remove_cv_t<U>>){
                HasBeenConverted = true;
                Converted_Type_Info = &typeid(U);
            }
            #endif
            return *this;
        }

        ~Shared() noexcept {
            Release();
        }
        
        
        void Release() {
            if (ctr_blk) {
                if (ctr_blk->ref_count.fetch_sub(1, std::memory_order_acq_rel) == 1) {
                    // Last strong ref — destroy the managed object
                    void* raw_ptr = ctr_blk->raw;
                    Traverse(*(Unqualified*)raw_ptr, [](auto& t){
                        if constexpr (!std::is_array_v<decltype(t)>) {
                            std::destroy_at(std::addressof(t));
                        }
                    });
                    ctr_blk->raw = nullptr;

                    // Free the object's storage when not co-allocated
                    if (!ctr_blk->co_allocated) {
                        ::operator delete(raw_ptr);
                    }

                    // Release the implicit weak reference held by strong refs
                    // If weak_count reaches 0, we delete the control block
                    if (ctr_blk->weak_count.fetch_sub(1, std::memory_order_acq_rel) == 1) {
                        if (ctr_blk->co_allocated) {
                            // CoAllocate: single aligned allocation
                            auto loc = ctr_blk->BaseLocation;
                            auto align_val = std::align_val_t{ctr_blk->alloc_align};
                            ctr_blk->~ControlBlock();
                            operator delete(loc, align_val);
                        } else {
                            // Manual: control block allocated separately
                            delete ctr_blk;
                        }
                    }
                }
                ctr_blk = nullptr;
            }
        }

        constexpr T* get() noexcept { return ctr_blk ? (T*)ctr_blk->raw : nullptr;}
        const T* get()const noexcept { return ctr_blk ? (T*)ctr_blk->raw : nullptr; }

        /**
         * @brief Unchecked Operation: This is dangerous if the control block is null. 
         * That happens only when the Shared has been created using the default constructor
         * 
         * @return T* The object managed by this structure.
         */
        T* operator->(){
            return (T*)ctr_blk->raw;
        }
        const T* operator->()const {
            return (const T*)ctr_blk->raw;
        }

        T& operator*(){ return *(T*)ctr_blk->raw; }
        const T& operator*()const { return *(T*)ctr_blk->raw; }

        uint_fast32_t UseCount() const noexcept { return ctr_blk ? ctr_blk->ref_count.load(std::memory_order_acquire) : 0; } 

        /**
         * @brief returns true if the control block has been created
         * 
         * or else returns false this Shared has no control block, should not use before initialization (or swapping with another valid Shared)
         */
        bool Constructed()const noexcept { return ctr_blk; }

        explicit operator bool() const noexcept {
            return ctr_blk && ctr_blk->raw; //rely on short-circuit to avoid UB.
        }

        template<typename U>
        constexpr void swap(Shared<U>& rhs) noexcept requires PolymorphicConvertible<U, T> {
            std::swap(this->ctr_blk, rhs.ctr_blk);
            #ifdef _DEBUG
            if constexpr (!std::is_same_v<std::remove_cv_t<T>, std::remove_cv_t<U>>){
                HasBeenConverted = true;
                Converted_Type_Info = &typeid(U);
            }
            #endif
        }

        friend Weak<T>;
        friend Handle<T>;
    };

    template<typename T> requires IsType<T>
    struct Weak{
    private:
        ControlBlock* ctr_blk = nullptr;
        
        // Allow all Weak<U> instantiations to access private members
        template<typename U> requires IsType<U>
        friend struct Weak;
        
        // Allow Shared to access private members
        template<typename U> requires IsType<U>
        friend struct Shared;
        
    public:
        constexpr Weak(const Shared<T>& s) noexcept : ctr_blk(s.ctr_blk) {
            if (ctr_blk) {
                ctr_blk->weak_count.fetch_add(1, std::memory_order_release);
            }
        }
        
        // Same-type copy constructor
        constexpr Weak(const Weak& other) noexcept : ctr_blk(other.ctr_blk) {
            if (ctr_blk) ctr_blk->weak_count.fetch_add(1, std::memory_order_release);
        }
        
        // Same-type move constructor
        constexpr Weak(Weak&& other) noexcept : ctr_blk(std::exchange(other.ctr_blk, nullptr)) {}
        
        // Same-type copy assignment
        Weak& operator=(const Weak& other) noexcept {
            if (this->ctr_blk == other.ctr_blk) return *this;
            Reset();
            ctr_blk = other.ctr_blk;
            if (ctr_blk) ctr_blk->weak_count.fetch_add(1, std::memory_order_release);
            return *this;
        }
        
        // Same-type move assignment
        Weak& operator=(Weak&& other) noexcept {
            if (this->ctr_blk == other.ctr_blk) return *this;
            Reset();
            ctr_blk = std::exchange(other.ctr_blk, nullptr);
            return *this;
        }
        
        // Converting constructor from Weak<U>
        template<typename U>
        constexpr Weak(const Weak<U>& other) noexcept requires PolymorphicConvertible<U, T> {
            ctr_blk = other.ctr_blk;
            if(ctr_blk) {
                ctr_blk->weak_count.fetch_add(1, std::memory_order_release);
            }
        }
        
        // Converting assignment operator
        template<typename U>
        Weak& operator=(const Weak<U>& other) noexcept requires PolymorphicConvertible<U, T> {
            if(this->ctr_blk == other.ctr_blk) return *this;
            Reset();
            ctr_blk = other.ctr_blk;
            if(ctr_blk) {
                ctr_blk->weak_count.fetch_add(1, std::memory_order_release);
            }
            return *this;
        }

        ~Weak(){
            Reset();
        }

        Shared<T> Lock() const noexcept {
            if (!ctr_blk) return Shared<T>();
            uint32_t count = ctr_blk->ref_count.load(std::memory_order_relaxed);
            do {
                if (count == 0) return Shared<T>();
            } while (!ctr_blk->ref_count.compare_exchange_weak(
                count, count + 1, std::memory_order_acq_rel, std::memory_order_relaxed));
            
            // ref_count is now atomically incremented — safe to construct
            Shared<T> result;
            result.ctr_blk = ctr_blk;
            // Don't increment again — we already did it in the CAS
            return result;
        }

        bool Expired()const noexcept{
            return ctr_blk && ctr_blk->ref_count.load(std::memory_order_acquire) == 0;
        }

        void Reset() noexcept{
            if (ctr_blk) {
                // With implicit weak reference protocol:
                // Only the thread that decrements weak_count to 0 deletes the control block
                // This eliminates the race between Release() and Reset()
                if (ctr_blk->weak_count.fetch_sub(1, std::memory_order_acq_rel) == 1) {
                    if (ctr_blk->co_allocated) {
                        // CoAllocate: single aligned allocation
                        auto loc = ctr_blk->BaseLocation;
                        auto align_val = std::align_val_t{ctr_blk->alloc_align};
                        ctr_blk->~ControlBlock();
                        operator delete(loc, align_val);
                    } else {
                        // Manual: control block allocated separately
                        delete ctr_blk;
                    }
                }
                ctr_blk = nullptr;
            }
        }

        uint_fast32_t UseCount()const noexcept{
            return ctr_blk ? ctr_blk->ref_count.load(std::memory_order_acquire) : 0;
        }

        explicit operator bool() const noexcept {
            return ctr_blk && ctr_blk->ref_count.load(std::memory_order_acquire) > 0 && ctr_blk->raw;
        }

        friend Shared<T>;
    };
    //Unique pointer equiv. Handle owns the object referenced. Planning on making a seperate container for Array (Handles for Arrays)
    template<typename T> requires IsType<T>
    struct Handle{
    private:
        T* raw = nullptr;
        
        // Allow all Handle<U> instantiations to access private members
        template<typename U> requires IsType<U>
        friend struct Handle;
        
    public:
        // NOTE: Constructors that allocate are noexcept and will terminate on OOM.
        constexpr Handle() noexcept = default;
        constexpr Handle(std::nullptr_t) {};
        // Object Construction
        // T construction
        constexpr Handle(T* t) noexcept {
            raw = t;
        }

        template<typename... Args>
        Handle(std::initializer_list<int> args, Args&&... extra) noexcept requires std::constructible_from<T, Args...> {
            raw = new T(args, std::forward<Args>(extra)...);
        }
        
        template<typename... Args>
        requires std::constructible_from<T, Args...> && 
                 (sizeof...(Args) > 0) && 
                 (!std::is_pointer_v<std::tuple_element_t<0, std::tuple<Args...>>>)
        explicit Handle(Args&&... args) noexcept
            : raw(new T(std::forward<Args>(args)...))
        {
        }
        
        template<typename... Args>
        explicit Handle(std::in_place_t, Args&&... args) noexcept
            : raw(new T(std::forward<Args>(args)...))
        {
        }

        //Polymorphic construction through U (Converting U to T safe-guarded by Concepts)
        template<typename U>
        constexpr explicit Handle(U* other) noexcept requires SafelyUpcastable<U, T> && !SameUnqualifiedType<T,U>{
            //If U is polymorphic => U*->T* => T::~T() must be virtual <=> T base of U

            //This is safe even without the cast as the concepts ensure only convertible types are allowed,
            //This is done work for explicit cast operators.
            raw = static_cast<T*>(other);
        }
        
        //End of Object Construction.

        template<typename U>
        constexpr Handle(Handle<U>&& other) noexcept requires PolymorphicConvertible<U, T>{
            reset();
            this->raw = static_cast<T*>(other.raw);
            other.raw = nullptr;
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
        template<typename U>
        Handle& operator=(Handle<U>&& mv) noexcept requires PolymorphicConvertible<U, T>{
            if(this->raw == static_cast<T*>(mv.raw)) return *this;
            reset();
            this->raw = static_cast<T*>(mv.raw);
            mv.raw = nullptr;
            return *this;
        }

        Handle(const Handle& cp) noexcept = delete;
        Handle& operator=(const Handle& cp) = delete;

        constexpr T* get() noexcept {return raw;}
        constexpr const T* get()const noexcept {return raw;}

        inline void reset(T* ptr = nullptr) noexcept{
            if(raw != ptr){
                if constexpr (std::is_array_v<T>){
                    delete[] raw;
                }else {
                    delete raw; //delete nullptr is both safe and defined as no-op.
                }
                raw = ptr;
            }
        }

        constexpr T* release() noexcept /*dangerous_release*/ {
            return std::exchange(raw, nullptr);
        }
        
        constexpr void swap(Handle& rhs) noexcept {
            std::swap(this->raw, rhs.raw);
        }

        Shared<T> ToShared(){
            if(raw == nullptr) return Shared<T>();
            Shared<T> shared;
            
            //I think at compile time this will be optimized.
            if constexpr (std::is_move_constructible_v<T>){
                shared = Shared<T>(std::move(*raw));
            }else if constexpr (std::is_copy_constructible_v<T>){
                shared = Shared<T>((const T&)*raw);
            }
            reset();
            // Guaranteed copy elision.
            return shared;
        }

        /**
         * @warning THIS IS DANGEROUS, It dereferences the pointer this object holds without checking.
         * 
         * @return constexpr T& 
         */
        constexpr inline T* operator->() noexcept{
            return raw;
        }
        /**
         * @warning THIS IS DANGEROUS, It dereferences the pointer this object holds without checking.
         * 
         * @return const T& 
         */
        constexpr inline const T* operator->()const noexcept{
            return raw;
        }
        /**
         * @warning THIS IS DANGEROUS, It dereferences the pointer this object holds without checking.
         * 
         * @return T& 
         */
        constexpr inline T& operator*() { return *raw;} //this can throw, it's the users responsibility to check for ptr validity.
        
        /**
         * @warning THIS IS DANGEROUS, It dereferences the pointer this object holds without checking.
         * 
         * @return const T& 
         */
        constexpr inline const T& operator*() const {return *raw;} //this can throw, it's the users responsibility to check for ptr validity.
        
        template<typename U>
        constexpr bool operator==(const Handle<U>& rhs)const noexcept {
            return this->raw == rhs.raw;
        }

        template<typename U>
        constexpr bool operator!=(const Handle<U>& rhs)const noexcept {
            return rhs.raw != this->raw;
        }
        //This is fine and left for simple checks.
        explicit constexpr inline operator bool()const noexcept{
            return this->raw;
        }

        constexpr inline bool operator!() const noexcept { return !this->raw; }
        
        //Explicitly disallow cast operators
        operator T*() = delete;
        operator void*() = delete;
    };

    // Specialization for bounded arrays: Handle<T[N]>
    template<typename T, size_t N> requires IsType<T>
    struct Handle<T[N]> {
        using element_type = T;
    private:
        T(*raw)[N] = nullptr;
        template<typename U> requires IsType<U>
        friend struct Handle;
    public:
        constexpr Handle() noexcept = default;
        constexpr Handle(std::nullptr_t) noexcept {}
        explicit Handle(T(*ptr)[N] = nullptr) noexcept : raw(ptr) {}

        ~Handle() noexcept { reset(); }

        Handle(Handle&& other) noexcept : raw(std::exchange(other.raw, nullptr)) {}
        Handle& operator=(Handle&& other) noexcept {
            reset(other.release());
            return *this;
        }

        Handle(const Handle&) = delete;
        Handle& operator=(const Handle&) = delete;

        T(*get())[N] { return raw; }
        const T(*get() const)[N] { return raw; }

        T& operator[](size_t idx) {
            assert(raw && "Handle<T[N]> dereference is null");
            assert(idx < N && "Handle<T[N]> index out of bounds");
            return (*raw)[idx];
        }
        const T& operator[](size_t idx) const {
            assert(raw && "Handle<T[N]> dereference is null");
            assert(idx < N && "Handle<T[N]> index out of bounds");
            return (*raw)[idx];
        }

        T(*release())[N] { return std::exchange(raw, nullptr); }

        void reset(T(*ptr)[N] = nullptr) noexcept {
            if (raw != ptr) {
                delete[] raw;
                raw = ptr;
            }
        }

        constexpr void swap(Handle& rhs) noexcept { std::swap(this->raw, rhs.raw); }

        explicit constexpr operator bool() const noexcept { return raw != nullptr; }
        constexpr inline bool operator!() const noexcept { return !raw; }
    };

    // Specialization for bounded arrays: Shared<T[N]>
    template<typename T, size_t N> requires IsType<T>
    struct Shared<T[N]> {
        using element_type = T;
        using Unqualified = std::remove_cv_t<T>;

        struct ControlBlock {
            T(*ptr)[N] = nullptr;
            std::atomic_uint32_t ref_count{1};
            std::atomic_uint32_t weak_count{1};
            void* BaseLocation = nullptr;
            size_t alloc_align = 0;

            explicit ControlBlock(T(*p)[N]) : ptr(p) {}
            ~ControlBlock() = default;
        };

        template<typename U> requires IsType<U>
        friend struct Shared;

        explicit Shared(T(*ptr)[N] = nullptr) noexcept {
            if (ptr) {
                ctr_blk = new ControlBlock(ptr);
            }
        }

        explicit Shared(void* ptr) noexcept {
            if (ptr) {
                ctr_blk = new ControlBlock((T(*)[N])ptr);
            }
        }

        ~Shared() noexcept { Release(); }

        void Release() {
            if (ctr_blk) {
                if (ctr_blk->ref_count.fetch_sub(1, std::memory_order_acq_rel) == 1) {
                    Traverse(*ctr_blk->ptr, [](auto& t) {
                        if constexpr (!std::is_array_v<decltype(t)>) {
                            std::destroy_at(std::addressof(t));
                        }
                    });
                    ::operator delete[](ctr_blk->ptr);

                    if (ctr_blk->weak_count.fetch_sub(1, std::memory_order_acq_rel) == 1) {
                        delete ctr_blk;
                    }
                }
                ctr_blk = nullptr;
            }
        }

        Shared(const Shared& other) noexcept : ctr_blk(other.ctr_blk) {
            if (ctr_blk) {
                ctr_blk->ref_count.fetch_add(1, std::memory_order_acq_rel);
            }
        }

        Shared& operator=(const Shared& other) noexcept {
            if (this != &other) {
                Release();
                ctr_blk = other.ctr_blk;
                if (ctr_blk) {
                    ctr_blk->ref_count.fetch_add(1, std::memory_order_acq_rel);
                }
            }
            return *this;
        }

        Shared(Shared&& other) noexcept : ctr_blk(std::exchange(other.ctr_blk, nullptr)) {}

        Shared& operator=(Shared&& other) noexcept {
            Release();
            ctr_blk = std::exchange(other.ctr_blk, nullptr);
            return *this;
        }

        T(*get())[N] { return ctr_blk ? ctr_blk->ptr : nullptr; }
        const T(*get() const)[N] { return ctr_blk ? ctr_blk->ptr : nullptr; }

        T(&operator*())[N] { return *get(); }
        const T(&operator*() const)[N] { return *get(); }

        T& operator[](size_t idx) {
            assert(get() && "Shared<T[N]> dereference is null");
            assert(idx < N && "Shared<T[N]> index out of bounds");
            return (*get())[idx];
        }
        const T& operator[](size_t idx) const {
            assert(get() && "Shared<T[N]> dereference is null");
            assert(idx < N && "Shared<T[N]> index out of bounds");
            return (*get())[idx];
        }

        explicit operator bool() const noexcept { return get() != nullptr; }

        uint32_t UseCount() const noexcept {
            return ctr_blk ? ctr_blk->ref_count.load(std::memory_order_acquire) : 0;
        }

        constexpr void swap(Shared& rhs) noexcept { std::swap(this->ctr_blk, rhs.ctr_blk); }

    private:
        ControlBlock* ctr_blk = nullptr;
    };

    // Specialization for bounded arrays: Weak<T[N]>
    template<typename T, size_t N> requires IsType<T>
    struct Weak<T[N]> {
        using element_type = T;
        using ControlBlock = typename Shared<T[N]>::ControlBlock;

    private:
        ControlBlock* ctr_blk = nullptr;

        template<typename U> requires IsType<U>
        friend struct Weak;
        template<typename U> requires IsType<U>
        friend struct Shared;

    public:
        constexpr Weak() noexcept = default;

        constexpr Weak(const Shared<T[N]>& s) noexcept : ctr_blk(s.ctr_blk) {
            if (ctr_blk) {
                ctr_blk->weak_count.fetch_add(1, std::memory_order_release);
            }
        }

        // Same-type copy constructor
        constexpr Weak(const Weak& other) noexcept : ctr_blk(other.ctr_blk) {
            if (ctr_blk) ctr_blk->weak_count.fetch_add(1, std::memory_order_release);
        }

        // Same-type move constructor
        constexpr Weak(Weak&& other) noexcept : ctr_blk(std::exchange(other.ctr_blk, nullptr)) {}

        // Same-type copy assignment
        Weak& operator=(const Weak& other) noexcept {
            if (this->ctr_blk == other.ctr_blk) return *this;
            Reset();
            ctr_blk = other.ctr_blk;
            if (ctr_blk) ctr_blk->weak_count.fetch_add(1, std::memory_order_release);
            return *this;
        }

        // Same-type move assignment
        Weak& operator=(Weak&& other) noexcept {
            if (this->ctr_blk == other.ctr_blk) return *this;
            Reset();
            ctr_blk = std::exchange(other.ctr_blk, nullptr);
            return *this;
        }

        ~Weak() { Reset(); }

        Shared<T[N]> Lock() const noexcept {
            if (!ctr_blk) return Shared<T[N]>();
            uint32_t count = ctr_blk->ref_count.load(std::memory_order_relaxed);
            do {
                if (count == 0) return Shared<T[N]>();
            } while (!ctr_blk->ref_count.compare_exchange_weak(
                count, count + 1, std::memory_order_acq_rel, std::memory_order_relaxed));

            Shared<T[N]> result;
            result.ctr_blk = ctr_blk;
            return result;
        }

        bool Expired() const noexcept {
            return ctr_blk && ctr_blk->ref_count.load(std::memory_order_acquire) == 0;
        }

        void Reset() noexcept {
            if (ctr_blk) {
                if (ctr_blk->weak_count.fetch_sub(1, std::memory_order_acq_rel) == 1) {
                    delete ctr_blk;
                }
                ctr_blk = nullptr;
            }
        }

        uint32_t UseCount() const noexcept {
            return ctr_blk ? ctr_blk->ref_count.load(std::memory_order_acquire) : 0;
        }

        explicit operator bool() const noexcept {
            return ctr_blk && ctr_blk->ref_count.load(std::memory_order_acquire) > 0 && ctr_blk->ptr;
        }
    };

    template <typename T>
    class Observer {
        static_assert(!std::is_reference_v<T>, "Observer<T> cannot be instantiated with a reference type");
    public:
        using element_type = T;

        // Constructors
        constexpr Observer() noexcept = default;
        constexpr Observer(std::nullptr_t) noexcept : ptr_(nullptr) {}

        constexpr explicit Observer(T* ptr) noexcept : ptr_(ptr) {}

        template <typename U> requires PolymorphicConvertible<U, T>
        constexpr Observer(U* ptr) noexcept : ptr_(ptr) {}

        // Copy constructor
        constexpr Observer(const Observer& other) noexcept = default;

        // Converting copy constructor
        template <typename U> requires PolymorphicConvertible<U, T>
        constexpr Observer(const Observer<U>& other) noexcept : ptr_(other.get()) {}

        // Assignment
        constexpr Observer& operator=(const Observer& other) noexcept = default;

        // Converting assignment
        template <typename U> requires PolymorphicConvertible<U, T>
        constexpr Observer& operator=(const Observer<U>& other) noexcept {
            ptr_ = other.get();
            return *this;
        }

        // Assignment from pointer
        constexpr Observer& operator=(T* ptr) noexcept {
            ptr_ = ptr;
            return *this;
        }

        // Modifiers
        constexpr void reset(T* ptr = nullptr) noexcept { ptr_ = ptr; }
        constexpr void release() noexcept { ptr_ = nullptr; }

        // Observers
        constexpr T* get() const noexcept { return ptr_; }
        constexpr T& operator*() const {
            assert(ptr_ && "Observer dereference is null");
            return *ptr_;
        }

        constexpr T* operator->() const {
            assert(ptr_ && "Observer dereference is null");
            return ptr_;
        }

        constexpr explicit operator bool() const noexcept { return ptr_ != nullptr; }

        // Comparisons
        constexpr auto operator<=>(const Observer&) const = default;

        template <typename U>
        constexpr bool operator==(const Observer<U>& other) const noexcept {
            return ptr_ == other.get();
        }

    private:
        T* ptr_ = nullptr;
    };

    // Deduction guide
    template <typename T> Handle(T*) -> Handle<T>;
    template <typename T> Shared(T*) -> Shared<T>;
    template <typename T> Shared(Weak<T>) -> Shared<T>;
    template <typename T> Observer(T*) -> Observer<T>;

    // Aliases
    template <typename T>
    using observer_ptr = Observer<T>;

}

template<typename T>
constexpr void swap(Hubris::Handle<T>& lhs, Hubris::Handle<T>& rhs) noexcept {
    lhs.swap(rhs);
};

template<typename T, size_t N>
constexpr void swap(Hubris::Handle<T[N]>& lhs, Hubris::Handle<T[N]>& rhs) noexcept {
    lhs.swap(rhs);
};

template<typename T>
constexpr void swap(Hubris::Shared<T>& lhs, Hubris::Shared<T>& rhs) noexcept {
    lhs.swap(rhs);
};

template<typename T, size_t N>
constexpr void swap(Hubris::Shared<T[N]>& lhs, Hubris::Shared<T[N]>& rhs) noexcept {
    lhs.swap(rhs);
};

template<typename T, typename U>
constexpr void swap(Hubris::Shared<T>& lhs, Hubris::Shared<U>& rhs) noexcept requires Hubris::PolymorphicConvertible<U, T> {
    lhs.swap(rhs);
}