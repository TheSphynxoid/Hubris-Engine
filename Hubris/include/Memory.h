#pragma once
#include <memory>
#include <version>
#include <cstddef>
#include <new>
#include <tuple>
#include <utility>
#include <type_traits>
#include <algorithm>

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
        std::atomic_uint32_t weak_count = { 1 };
        void* BaseLocation;
        size_t alloc_align;
    };

    template<typename T>
    consteval size_t padded_size() {
        return (sizeof(T) + alignof(T) - 1) & ~(alignof(T) - 1);
    }

    
    template<typename T, typename F>
    void Traverse(T&& container, F&& func) {
        if constexpr (std::ranges::range<T> && !std::is_same_v<std::decay_t<T>, std::string>) {
            for (auto&& elem : container) {
                Traverse(std::forward<decltype(elem)>(elem), func);
            }
        } else {
            func(std::forward<T>(container));
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
            ? operator new(padded_size<ControlBlock>() + sizeof(T), std::align_val_t{ alloc_align })
            : operator new(padded_size<T>() + sizeof(ControlBlock), std::align_val_t{ alloc_align }));
        
        //This is aligned because either it is at the start (alignof(Crt) > alignof(T)) or T is padded at the end to align make this aligned
        ControlBlock* ctr = new(ptr + CTR_offset)ControlBlock();
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
        constexpr size_t Toffset =  alignof(ControlBlock) > alignof(T) ? padded_size<ControlBlock>() : 0;
        ControlBlock* ctr = CoAllocate_Unsafe<T>();
        Construct_T_Inplace<Args...>(ctr->raw, std::forward<Args>(args)...);
        return ctr;
    }

    // template<typename T>
    // constexpr void CoDelete(ControlBlock* crt){
    //     assert(crt->raw && "ControlBlock is observing no object");
    //     if constexpr(!std::is_array_v<T>){

    //     }
    // }
    
    template<typename T> requires IsType<T>
    struct Shared {
        static_assert(!std::is_reference_v<T>, "No reference is allowed.");
        using Unqualified = std::remove_cv_t<T>;
        #ifdef _DEBUG
        bool HasBeenConverted = false;
        const type_info& Converted_Type_Info = typeid(T);
        #endif
    private:
        /// @brief The control block must never be assigned by the user, 
        /// it must be verified that the underlying types before assigning as it is abstracted through a void* to allow for conversion (only polymorphic conversion is allowed).
        ControlBlock* ctr_blk = nullptr;

        /// @brief Create a Shared pointer wrapped around an arbitrary pointer. This is dangerous, thus it's private.
        /// @param t Pointer to T.
        constexpr Shared(T* t) noexcept {
            ctr_blk = new ControlBlock;
            ctr_blk->raw = (void*)t;
        }

        /// @brief Create a shared pointer with a controlblock, this is only useful for Weak<T>
        /// @param ctr the shared control block.
        constexpr Shared(ControlBlock* ctr) noexcept{
            assert(ctr && "Attempted to create a Shared with an empty ControlBlock");
            ctr_blk = ctr;
            if(!ctr_blk)return; //Pre-caution, but a stupid case.
            ctr_blk->ref_count.fetch_add(1, std::memory_order_release);
        }

    public:
        constexpr Shared() noexcept = default;

        template<typename ...Args>
        constexpr Shared(Args&& ...args){
            //For now, CoAllocates blocks unbounded arrays. So calling dtor is possible by indexing the array.
            ctr_blk = CoAllocate<std::remove_cv_t<T>>(std::forward<Args>(args)...);
        }

        constexpr explicit Shared(Weak<T>&& promote) noexcept{
            if(promote.Expired())return;
            this->ctr_blk = promote.ctr_blk;
            ctr_blk->ref_count.fetch_add(1, std::memory_order_release);
            promote.Reset();
        }

        template<typename U>
        constexpr explicit Shared(const Shared<U>& other) noexcept requires PolymorphicConvertible<U, T> {
            this->ctr_blk = other.ctr_blk;
            this->ctr_blk->ref_count.fetch_add(1, std::memory_order_release);
            #ifdef _DEBUG
            if constexpr (!std::is_same_v<std::remove_cv_t<T>, std::remove_cv_t<U>>){
                HasBeenConverted = true;
                Converted_Type_Info = typeid(U);
            }
            #endif
        }
                
        template<typename U>
        Shared& operator=(const Shared<U>& cpy) noexcept requires PolymorphicConvertible<U, T>{
            if(this == &cpy)return *this;
            Release();
            this->ctr_blk = cpy.ctr_blk;
            //Copied an "empty" Shared into this one. User at fault but still we do what they wanted.
            if(!ctr_blk)return;
            ctr_blk->ref_count.fetch_add(1, std::memory_order_acquire);
            #ifdef _DEBUG
            if constexpr (!std::is_same_v<std::remove_cv_t<T>, std::remove_cv_t<U>>){
                HasBeenConverted = true;
                Converted_Type_Info = typeid(U);
            }
            #endif
            return *this;
        }

        template<typename U>
        constexpr explicit Shared(Shared<U>&& other) noexcept requires PolymorphicConvertible<U, T> {
            std::swap(this->ctr_blk, other->crt_blk);
            #ifdef _DEBUG
            if constexpr (!std::is_same_v<std::remove_cv_t<T>, std::remove_cv_t<U>>){
                HasBeenConverted = true;
                Converted_Type_Info = typeid(U);
            }
            #endif
        }
        
        template<typename U>
        Shared& operator=(Shared<U>&& mv) noexcept requires PolymorphicConvertible<U, T> {
            if(this == &mv)return *this;
            Release();
            std::swap(this->ctr_blk, mv.ctr_blk);
            #ifdef _DEBUG
            if constexpr (!std::is_same_v<std::remove_cv_t<T>, std::remove_cv_t<U>>){
                HasBeenConverted = true;
                Converted_Type_Info = typeid(U);
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
                    Traverse(*(Unqualified*)ctr_blk->raw, [](std::remove_all_extents_t<Unqualified>& t){
                        std::destroy_at(std::addressof(t));
                    });
                    ctr_blk->raw = nullptr;
                    // }
                    if (ctr_blk->weak_count.load(std::memory_order_acq_rel) == 0) {
                        auto loc = ctr_blk->BaseLocation;
                        auto align_val = std::align_val_t{ctr_blk->alloc_align};
                        ctr_blk->~ControlBlock();
                        operator delete(loc, align_val);
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

        operator bool(){
            return ctr_blk && ctr_blk->raw; //rely on short-circuit to avoid UB.
        }

        template<typename U>
        constexpr void swap(Shared<U>& rhs) noexcept requires PolymorphicConvertible<U, T> {
            std::swap(this->ctr_blk, rhs.ctr_blk);
            #ifdef _DEBUG
            if constexpr (!std::is_same_v<std::remove_cv_t<T>, std::remove_cv_t<U>>){
                HasBeenConverted = true;
                Converted_Type_Info = typeid(U);
            }
            #endif
        }

        friend Weak<T>;
        friend Handle<T>;
    };

    template<typename T> requires IsType<T>
    struct Weak{
    private:
        ControlBlock* ctr_blk;
    public:
        constexpr Weak(const Shared<T>& s) noexcept{
            // static_assert(s.ctr_blk != nullptr, "Attempted to create a weak reference from an empty Shared");
            ctr_blk = s.ctr_blk;
            //This is memory order release instead of relax because it is creating a Weak reference and it must be valid upon return.
            //in the case of other operation, whoever deletes the control block doesn't matter as long it is deleted,
            //the deletion is ensure by the atomicity of the counter, but order doesn't matter. Correct me if i'm wrong.
            ctr_blk->weak_count.fetch_add(1, std::memory_order_release);
        }

        ~Weak(){
            Reset();
        }

        Shared<T> Lock()const noexcept {
            if(ctr_blk->ref_count.load(std::memory_order_acquire) == 0)return Shared<T>();
            return Shared<T>(*this);
        }

        bool Expired()const noexcept{
            return ctr_blk && ctr_blk->ref_count.load(std::memory_order_acquire) == 0;
        }

        void Reset() noexcept{
            if (ctr_blk) {
                //This condition, decrements the weak_count, so even if the ctr_blk isn't expired, the counter still got down.
                if (ctr_blk->weak_count.fetch_sub(1, std::memory_order_relaxed) == 1 &&
                    ctr_blk->ref_count.load(std::memory_order_acquire) == 0) {
                    auto loc = ctr_blk->BaseLocation;
                    auto align_val = std::align_val_t{ctr_blk->alloc_align};
                    ctr_blk->~ControlBlock();
                    operator delete(loc, align_val);
                }
                ctr_blk = nullptr;
            }
        }

        uint_fast32_t UseCount()const noexcept{
            return ctr_blk ? ctr_blk->ref_count.load(std::memory_order_acquire) : 0;
        }

        operator bool()const noexcept{
            return !Expired() && ctr_blk->raw;
        }

        friend Shared<T>;
    };
    //Unique pointer equiv. Handle owns the object referenced. Planning on making a seperate container for Array (Handles for Arrays)
    template<typename T> requires IsType<T>
    struct Handle{
    private:
        T* raw = nullptr;
    public:
        constexpr Handle() noexcept = default;
        constexpr Handle(std::nullptr_t) {};
        constexpr Handle(T* t) noexcept {
            raw = t;
        }

        template<typename U>
        constexpr explicit Handle(U* other) noexcept requires SafelyUpcastable<U, T> && !SameUnqualifiedType<T,U>{
            //If U is polymorphic => U*->T* => T::~T() must be virtual <=> T base of U

            //This is safe even without the cast as the concepts ensure only convertible types are allowed,
            //This is done work for explicit cast operators.
            raw = static_cast<T*>(other);
        }
        
        template<typename U>
        constexpr Handle(Handle<U>&& other) noexcept requires Castable<U*, T*>{
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
        template<typename U>
        Handle& operator=(Handle<U>&& mv) noexcept requires PolymorphicConvertible<U, T>{
            if(this == &mv)return *this;
            reset();
            std::swap(this->raw, static_cast<T*>(mv.raw));
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
        
        template<typename U>
        constexpr void swap(Handle<U>& rhs) noexcept requires PolymorphicConvertible<U, T> {
            std::swap(this->raw, static_cast<T*>(rhs.raw));
        }

        Shared<T> ToShared(){
            if(raw == nullptr) return Shared<T>();
            Shared<T> shared;
            //I think at compile time this will be optimized.
            if constexpr (std::is_trivially_copyable_v<T>){
                shared.ctr_blk = CoAllocate_Unsafe<T>();
                //Assumed safe because CoAllocate Allocates size T and we're moving in it.
                memmove(shared.ctr_blk->raw, raw, sizeof(T));
            }else if constexpr (std::is_move_constructible_v<T>){
                shared = Shared<T>(std::move(*raw));
            }else if constexpr (std::is_copy_constructible_v<T>){
                shared = Shared<T>((const T&)*raw);
            }
            reset();
            //without RVO, this will be a slightly costly. Ctor(inc)->Copy(inc.)->Delete(dec.) will be called.
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
        constexpr inline operator bool()const noexcept{
            return this->raw;
        }

        constexpr inline bool operator!() const noexcept { return !this->raw; }
        
        //Explicitly disallow cast operators
        operator T*() = delete;
        operator void*() = delete;
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

template<typename T, typename U>
constexpr void swap(Hubris::Handle<T>& lhs, Hubris::Handle<U>& rhs) noexcept requires Hubris::PolymorphicConvertible<U, T> {
    lhs.swap(rhs);
}

template<typename T>
constexpr void swap(Hubris::Shared<T>& lhs, Hubris::Shared<T>& rhs) noexcept {
    lhs.swap(rhs);
};

template<typename T, typename U>
constexpr void swap(Hubris::Shared<T>& lhs, Hubris::Shared<U>& rhs) noexcept requires Hubris::PolymorphicConvertible<U, T> {
    lhs.swap(rhs);
}