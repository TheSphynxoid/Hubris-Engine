#pragma once
#include <cstddef>
#include <utility>
#include <initializer_list>
#include <type_traits>
#include <limits>
#include <cstdlib>
#include <new>

//export module Containers;

namespace Hubris {
    //Store doesn't keep any information besides the underlaying buffer. It isn't meant for external access.
    //BasicStore is strictly a low-level buffer that assumes you know the bounds externally. You would use this like you'd use a void*
    //This design may be discarded for a more integrated variant of the containers instead of the store changing.
    template<typename T>
    struct BasicStore {
    private:
        T* Backing = nullptr;
    public:
        BasicStore(size_t count) {
            Backing = (T*)std::malloc(count * sizeof(T));
        }

        ~BasicStore() noexcept {
            if (!Backing) {
#ifdef DEBUG
                __debugbreak();
#endif
                return;
            }
            deallocate();
        }

        void deallocate() noexcept {
            std::free(Backing);
            Backing = nullptr;
        }

        operator bool()const noexcept {
            return (bool)Backing;
        }

        //This is unsafe by principle.
        T& operator[](size_t index) const {
            return Backing[index];
        }
    };

    /**
     * @brief A generic dynamic array container that manages a sequence of elements of type T, providing memory management, element access, and basic list operations with error handling.
     * @tparam T The type of elements stored in the list.
     * @note: Remove Iterators or reimplement them.
     */
    template<typename T>
    class List {
    public:
        using value_type = T;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;
        using reference = T&;
        using const_reference = const T&;
        using pointer = T*;
        using const_pointer = const T*;
        using iterator = T*;
        using const_iterator = const T*;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        // Result type for operations that can fail
        enum class Result {
            Success,
            OutOfMemory,
            OutOfRange,
            InvalidArgument
        };

    private:
        T* m_data;
        size_type m_size;
        size_type m_capacity;
        bool m_valid; // Tracks if the container is in a valid state

        // Growth factor: 1.5 like MSVC
        static constexpr size_type growth_factor_numerator = 3;
        static constexpr size_type growth_factor_denominator = 2;

        void deallocate() noexcept {
            if (m_data) {
                std::free(m_data);
                m_data = nullptr;
            }
        }

        T* allocate_memory(size_type count) noexcept {
            if (count == 0) return nullptr;

            // Check for overflow
            if (count > max_size()) {
                return nullptr;
            }

            // Check for multiplication overflow
            if (count > SIZE_MAX / sizeof(T)) {
                return nullptr;
            }

            void* ptr = std::malloc(count * sizeof(T));
            return static_cast<T*>(ptr);
        }

        void destroy_range(T* first, T* last) noexcept {
            for (; first != last; ++first) {
                first->~T();
            }
        }

        void destroy_all() noexcept {
            if (m_data) {
                destroy_range(m_data, m_data + m_size);
            }
        }

        // Calculate new capacity with 1.5x growth, returns 0 on overflow
        size_type calculate_growth(size_type min_capacity) const noexcept {
            const size_type old_capacity = m_capacity;
            const size_type max_cap = max_size();

            if (min_capacity > max_cap) {
                return 0; // Overflow
            }

            if (old_capacity > max_cap - old_capacity / 2) {
                return max_cap; // Avoid overflow, use max
            }

            const size_type new_capacity = old_capacity + old_capacity / 2;
            return (new_capacity < min_capacity) ? min_capacity : new_capacity;
        }

        // Reallocate and move/copy existing elements
        Result reallocate(size_type new_capacity) noexcept {
            if (new_capacity == 0) {
                destroy_all();
                deallocate();
                m_data = nullptr;
                m_size = 0;
                m_capacity = 0;
                return Result::Success;
            }

            T* new_data = allocate_memory(new_capacity);
            if (!new_data) {
                return Result::OutOfMemory;
            }

            // Move or copy existing elements
            if (m_data) {
                bool move_success = true;

                if constexpr (std::is_nothrow_move_constructible_v<T>) {
                    move_success = uninitialized_move(m_data, m_data + m_size, new_data);
                }
                else {
                    move_success = uninitialized_copy(m_data, m_data + m_size, new_data);
                }

                if (!move_success) {
                    std::free(new_data);
                    return Result::OutOfMemory;
                }

                destroy_all();
                deallocate();
            }

            m_data = new_data;
            m_capacity = new_capacity;
            return Result::Success;
        }

        // Uninitialized construction helpers - return false on failure
        template<typename InputIt>
        bool uninitialized_copy(InputIt first, InputIt last, T* dest) noexcept {
            T* current = dest;
            for (; first != last; ++first, ++current) {
                if constexpr (std::is_nothrow_copy_constructible_v<T>) {
                    new (current) T(*first);
                }
                else {
                    // For types that can throw, we need to be more careful
                    // In a game engine, you might want to restrict to nothrow types
                    new (current) T(*first);
                }
            }
            return true;
        }

        template<typename InputIt>
        bool uninitialized_move(InputIt first, InputIt last, T* dest) noexcept {
            T* current = dest;
            for (; first != last; ++first, ++current) {
                new (current) T(std::move(*first));
            }
            return true;
        }

        template<typename... Args>
        bool construct_at(T* ptr, Args&&... args) noexcept {
            if constexpr (std::is_nothrow_constructible_v<T, Args...>) {
                new (ptr) T(std::forward<Args>(args)...);
                return true;
            }
            else {
                // For game engines, you might want to static_assert this away
                new (ptr) T(std::forward<Args>(args)...);
                return true;
            }
        }

        // Make invalid (used when construction fails)
        void make_invalid() noexcept {
            destroy_all();
            deallocate();
            m_data = nullptr;
            m_size = 0;
            m_capacity = 0;
            m_valid = false;
        }

    public:
        // Constructors
        List() noexcept : m_data(nullptr), m_size(0), m_capacity(0), m_valid(true) {}

        explicit List(size_type count) noexcept : m_data(nullptr), m_size(0), m_capacity(0), m_valid(true) {
            if (count > 0) {
                m_data = allocate_memory(count);
                if (!m_data) {
                    m_valid = false;
                    return;
                }
                m_capacity = count;

                for (size_type i = 0; i < count; ++i) {
                    if (!construct_at(m_data + i)) {
                        destroy_range(m_data, m_data + i);
                        make_invalid();
                        return;
                    }
                    ++m_size;
                }
            }
        }

        List(size_type count, const T& value) noexcept : m_data(nullptr), m_size(0), m_capacity(0), m_valid(true) {
            if (count > 0) {
                m_data = allocate_memory(count);
                if (!m_data) {
                    m_valid = false;
                    return;
                }
                m_capacity = count;

                for (size_type i = 0; i < count; ++i) {
                    if (!construct_at(m_data + i, value)) {
                        destroy_range(m_data, m_data + i);
                        make_invalid();
                        return;
                    }
                    ++m_size;
                }
            }
        }

        template<typename InputIt>
        List(InputIt first, InputIt last) noexcept : m_data(nullptr), m_size(0), m_capacity(0), m_valid(true) {
            if constexpr (std::is_same_v<typename std::iterator_traits<InputIt>::iterator_category,
                std::random_access_iterator_tag>) {
                const size_type count = std::distance(first, last);
                if (count > 0) {
                    m_data = allocate_memory(count);
                    if (!m_data) {
                        m_valid = false;
                        return;
                    }
                    m_capacity = count;
                    if (!uninitialized_copy(first, last, m_data)) {
                        make_invalid();
                        return;
                    }
                    m_size = count;
                }
            }
            else {
                // For non-random access iterators, push_back one by one
                for (; first != last; ++first) {
                    if (push_back(*first) != Result::Success) {
                        make_invalid();
                        return;
                    }
                }
            }
        }

        List(std::initializer_list<T> init) noexcept : List(init.begin(), init.end()) {}

        // Copy constructor
        List(const List& other) noexcept : m_data(nullptr), m_size(0), m_capacity(0), m_valid(true) {
            if (!other.m_valid) {
                m_valid = false;
                return;
            }

            if (other.m_size > 0) {
                m_data = allocate_memory(other.m_size);
                if (!m_data) {
                    m_valid = false;
                    return;
                }
                m_capacity = other.m_size;
                if (!uninitialized_copy(other.m_data, other.m_data + other.m_size, m_data)) {
                    make_invalid();
                    return;
                }
                m_size = other.m_size;
            }
        }

        // Move constructor
        List(List&& other) noexcept
            : m_data(other.m_data), m_size(other.m_size), m_capacity(other.m_capacity), m_valid(other.m_valid) {
            other.m_data = nullptr;
            other.m_size = 0;
            other.m_capacity = 0;
            other.m_valid = true; // Moved-from object is valid but empty
        }

        // Destructor
        ~List() {
            destroy_all();
            deallocate();
        }

        // Validity check
        bool is_valid() const noexcept {
            return m_valid;
        }

        // Assignment operators
        List& operator=(const List& other) noexcept {
            if (this != &other) {
                if (!other.m_valid) {
                    make_invalid();
                    return *this;
                }

                if (other.m_size <= m_capacity) {
                    // Reuse existing storage
                    size_type common_size = std::min(m_size, other.m_size);

                    // Copy assign existing elements
                    for (size_type i = 0; i < common_size; ++i) {
                        m_data[i] = other.m_data[i];
                    }

                    // Construct additional elements
                    for (size_type i = common_size; i < other.m_size; ++i) {
                        if (!construct_at(m_data + i, other.m_data[i])) {
                            destroy_range(m_data + common_size, m_data + i);
                            m_size = common_size;
                            make_invalid();
                            return *this;
                        }
                    }

                    // Destroy excess elements
                    destroy_range(m_data + other.m_size, m_data + m_size);

                    m_size = other.m_size;
                }
                else {
                    // Need to reallocate
                    List temp(other);
                    if (!temp.m_valid) {
                        make_invalid();
                        return *this;
                    }
                    swap(temp);
                }
            }
            return *this;
        }

        List& operator=(List&& other) noexcept {
            if (this != &other) {
                destroy_all();
                deallocate();

                m_data = other.m_data;
                m_size = other.m_size;
                m_capacity = other.m_capacity;
                m_valid = other.m_valid;

                other.m_data = nullptr;
                other.m_size = 0;
                other.m_capacity = 0;
                other.m_valid = true;
            }
            return *this;
        }

        List& operator=(std::initializer_list<T> init) noexcept {
            assign(init.begin(), init.end());
            return *this;
        }

        // Assign
        Result assign(size_type count, const T& value) noexcept {
            if (!m_valid) return Result::InvalidArgument;

            if (count <= m_capacity) {
                size_type common_size = std::min(m_size, count);

                for (size_type i = 0; i < common_size; ++i) {
                    m_data[i] = value;
                }

                for (size_type i = common_size; i < count; ++i) {
                    if (!construct_at(m_data + i, value)) {
                        destroy_range(m_data + common_size, m_data + i);
                        m_size = common_size;
                        return Result::OutOfMemory;
                    }
                }

                destroy_range(m_data + count, m_data + m_size);
                m_size = count;
                return Result::Success;
            }
            else {
                List temp(count, value);
                if (!temp.m_valid) {
                    return Result::OutOfMemory;
                }
                swap(temp);
                return Result::Success;
            }
        }

        template<typename InputIt>
        Result assign(InputIt first, InputIt last) noexcept {
            if (!m_valid) return Result::InvalidArgument;

            List temp(first, last);
            if (!temp.m_valid) {
                return Result::OutOfMemory;
            }
            swap(temp);
            return Result::Success;
        }

        Result assign(std::initializer_list<T> init) noexcept {
            return assign(init.begin(), init.end());
        }

        // Element access with bounds checking
        Result at(size_type pos, reference& out) noexcept {
            if (!m_valid) return Result::InvalidArgument;
            if (pos >= m_size) return Result::OutOfRange;
            out = m_data[pos];
            return Result::Success;
        }

        Result at(size_type pos, const_reference& out) const noexcept {
            if (!m_valid) return Result::InvalidArgument;
            if (pos >= m_size) return Result::OutOfRange;
            out = m_data[pos];
            return Result::Success;
        }

        // Unchecked element access (for performance)
        reference operator[](size_type pos) noexcept {
            return m_data[pos];
        }

        const_reference operator[](size_type pos) const noexcept {
            return m_data[pos];
        }

        // Safe element access returning pointers (nullptr on failure)
        T* get(size_type pos) noexcept {
            if (!m_valid || pos >= m_size) return nullptr;
            return &m_data[pos];
        }

        const T* get(size_type pos) const noexcept {
            if (!m_valid || pos >= m_size) return nullptr;
            return &m_data[pos];
        }

        reference front() noexcept {
            return m_data[0];
        }

        const_reference front() const noexcept {
            return m_data[0];
        }

        reference back() noexcept {
            return m_data[m_size - 1];
        }

        const_reference back() const noexcept {
            return m_data[m_size - 1];
        }

        T* data() noexcept {
            return m_data;
        }

        const T* data() const noexcept {
            return m_data;
        }

        // Iterators
        iterator begin() noexcept { return m_data; }
        const_iterator begin() const noexcept { return m_data; }
        const_iterator cbegin() const noexcept { return m_data; }

        iterator end() noexcept { return m_data + m_size; }
        const_iterator end() const noexcept { return m_data + m_size; }
        const_iterator cend() const noexcept { return m_data + m_size; }

        reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
        const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }
        const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(end()); }

        reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
        const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }
        const_reverse_iterator crend() const noexcept { return const_reverse_iterator(begin()); }

        // Capacity
        bool empty() const noexcept {
            return m_size == 0;
        }

        size_type size() const noexcept {
            return m_size;
        }

        size_type max_size() const noexcept {
            return std::numeric_limits<size_type>::max() / sizeof(T);
        }

        Result reserve(size_type new_capacity) noexcept {
            if (!m_valid) return Result::InvalidArgument;
            if (new_capacity > m_capacity) {
                return reallocate(new_capacity);
            }
            return Result::Success;
        }

        size_type capacity() const noexcept {
            return m_capacity;
        }

        Result shrink_to_fit() noexcept {
            if (!m_valid) return Result::InvalidArgument;
            if (m_size < m_capacity) {
                return reallocate(m_size);
            }
            return Result::Success;
        }

        // Modifiers
        void clear() noexcept {
            if (m_valid) {
                destroy_all();
                m_size = 0;
            }
        }

        Result push_back(const T& value) noexcept {
            if (!m_valid) return Result::InvalidArgument;

            if (m_size == m_capacity) {
                size_type new_capacity = calculate_growth(m_size + 1);
                if (new_capacity == 0) return Result::OutOfMemory;

                Result result = reallocate(new_capacity);
                if (result != Result::Success) return result;
            }

            if (!construct_at(m_data + m_size, value)) {
                return Result::OutOfMemory;
            }
            ++m_size;
            return Result::Success;
        }

        Result push_back(T&& value) noexcept {
            if (!m_valid) return Result::InvalidArgument;

            if (m_size == m_capacity) {
                size_type new_capacity = calculate_growth(m_size + 1);
                if (new_capacity == 0) return Result::OutOfMemory;

                Result result = reallocate(new_capacity);
                if (result != Result::Success) return result;
            }

            if (!construct_at(m_data + m_size, std::move(value))) {
                return Result::OutOfMemory;
            }
            ++m_size;
            return Result::Success;
        }

        template<typename... Args>
        Result emplace_back(Args&&... args) noexcept {
            if (!m_valid) return Result::InvalidArgument;

            if (m_size == m_capacity) {
                size_type new_capacity = calculate_growth(m_size + 1);
                if (new_capacity == 0) return Result::OutOfMemory;

                Result result = reallocate(new_capacity);
                if (result != Result::Success) return result;
            }

            if (!construct_at(m_data + m_size, std::forward<Args>(args)...)) {
                return Result::OutOfMemory;
            }
            ++m_size;
            return Result::Success;
        }

        Result pop_back() noexcept {
            if (!m_valid) return Result::InvalidArgument;
            if (m_size == 0) return Result::OutOfRange;

            --m_size;
            m_data[m_size].~T();
            return Result::Success;
        }

        Result resize(size_type count) noexcept {
            if (!m_valid) return Result::InvalidArgument;

            if (count < m_size) {
                destroy_range(m_data + count, m_data + m_size);
                m_size = count;
            }
            else if (count > m_size) {
                if (count > m_capacity) {
                    Result result = reallocate(count);
                    if (result != Result::Success) return result;
                }

                for (size_type i = m_size; i < count; ++i) {
                    if (!construct_at(m_data + i)) {
                        destroy_range(m_data + m_size, m_data + i);
                        return Result::OutOfMemory;
                    }
                }
                m_size = count;
            }
            return Result::Success;
        }

        Result resize(size_type count, const T& value) noexcept {
            if (!m_valid) return Result::InvalidArgument;

            if (count < m_size) {
                destroy_range(m_data + count, m_data + m_size);
                m_size = count;
            }
            else if (count > m_size) {
                if (count > m_capacity) {
                    Result result = reallocate(count);
                    if (result != Result::Success) return result;
                }

                for (size_type i = m_size; i < count; ++i) {
                    if (!construct_at(m_data + i, value)) {
                        destroy_range(m_data + m_size, m_data + i);
                        return Result::OutOfMemory;
                    }
                }
                m_size = count;
            }
            return Result::Success;
        }

        void swap(List& other) noexcept {
            std::swap(m_data, other.m_data);
            std::swap(m_size, other.m_size);
            std::swap(m_capacity, other.m_capacity);
            std::swap(m_valid, other.m_valid);
        }

        // Insert operations (simplified - you can expand these)
        Result insert(size_type pos, const T& value) noexcept {
            if (!m_valid) return Result::InvalidArgument;
            if (pos > m_size) return Result::OutOfRange;

            if (m_size == m_capacity) {
                size_type new_capacity = calculate_growth(m_size + 1);
                if (new_capacity == 0) return Result::OutOfMemory;

                Result result = reallocate(new_capacity);
                if (result != Result::Success) return result;
            }

            // Move elements to make space
            for (size_type i = m_size; i > pos; --i) {
                if (i == m_size) {
                    if (!construct_at(m_data + i, std::move(m_data[i - 1]))) {
                        return Result::OutOfMemory;
                    }
                }
                else {
                    m_data[i] = std::move(m_data[i - 1]);
                }
            }

            // Insert new element
            if (pos < m_size) {
                m_data[pos] = value;
            }
            else {
                if (!construct_at(m_data + pos, value)) {
                    return Result::OutOfMemory;
                }
            }

            ++m_size;
            return Result::Success;
        }

        Result erase(size_type pos) noexcept {
            if (!m_valid) return Result::InvalidArgument;
            if (pos >= m_size) return Result::OutOfRange;

            // Move elements down
            for (size_type i = pos; i < m_size - 1; ++i) {
                m_data[i] = std::move(m_data[i + 1]);
            }

            // Destroy last element
            --m_size;
            m_data[m_size].~T();
            return Result::Success;
        }
    };

    // Non-member functions
    template<typename T>
    bool operator==(const List<T>& lhs, const List<T>& rhs) noexcept {
        if (!lhs.is_valid() || !rhs.is_valid()) return false;

        if (lhs.size() != rhs.size()) return false;

        for (size_t i = 0; i < lhs.size(); ++i) {
            if (lhs[i] != rhs[i]) return false;
        }
        return true;
    }

    template<typename T>
    bool operator!=(const List<T>& lhs, const List<T>& rhs) noexcept {
        return !(lhs == rhs);
    }

    template<typename T>
    void swap(List<T>& lhs, List<T>& rhs) noexcept {
        lhs.swap(rhs);
    }

    // Utility functions for checking results
    inline bool is_success(List<int>::Result result) noexcept {
        return result == List<int>::Result::Success;
    }

    inline bool is_failure(List<int>::Result result) noexcept {
        return result != List<int>::Result::Success;
    }
}