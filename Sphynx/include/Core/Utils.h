#include <stdint.h>		// for uint32_t
#include <limits.h>		// for CHAR_BIT
// #define NDEBUG
#include <assert.h>
#include <atomic>
#include <type_traits>

//

static inline uint32_t rotl32(uint32_t n, unsigned int c)
{
	const unsigned int mask = (CHAR_BIT * sizeof(n) - 1);  // assumes width is a power of 2.

	// assert ( (c<=mask) &&"rotate by type width or more");
	c &= mask;
	return (n << c) | (n >> ((-c) & mask));
}

static inline uint32_t rotr32(uint32_t n, unsigned int c)
{
	const unsigned int mask = (CHAR_BIT * sizeof(n) - 1);

	// assert ( (c<=mask) &&"rotate by type width or more");
	c &= mask;
	return (n >> c) | (n << ((-c) & mask));
}
namespace Sphynx{

    /**
     * @brief An implementation of a fixed-size (power of two only) Ring Buffer.
     * @tparam T The Buffer's underlying type.
     * @tparam buffsize The buffer size, must be a power of two.
     */
    template<typename T, size_t buffsize>
    class RingBuffer {
    private:
        //Safety checks.
        static_assert((buffsize > 0) && ((buffsize& (buffsize - 1)) == 0),
            "Buffer size must be a power of 2 to gain performance.");
        static_assert((!std::is_abstract_v<T>), "Abstract types can cause issues.");
        static_assert((!std::is_array_v<T>), "Cannot store arrays in the ring buffer. Although, support maybe added if a need ever arises.");
        //Fixed-size
        std::array<T, buffsize> buffer;
        std::atomic<size_t> head{ 0 };				// Head (dequeue pointer)
        std::atomic<size_t> tail{ 0 };				// Tail (enqueue pointer)
        const size_t mask = buffsize - 1;	// Mask used for fast wrap-around
    public:
        /**
         * @brief Evaluated at compile time, returns the size of the buffer which is already known.
         */
        constexpr size_t Capacity()const noexcept { return buffsize; };
        RingBuffer()noexcept = default;
        ~RingBuffer()noexcept = default;

        // Clear the buffer
        void Clear() noexcept {
            head.store(0, std::memory_order_relaxed);
            tail.store(0, std::memory_order_relaxed);
            //buffer = std::array<T, buffsize>();
        }

        // Enqueue element into the buffer, returns false if the buffer is full
        bool Enqueue(const T& element) noexcept {
            size_t current_tail = tail.load(std::memory_order_relaxed);
            size_t next_tail = (current_tail + 1) & mask;

            if (next_tail == head.load(std::memory_order_acquire)) {
                // Buffer is full
                return false;
            }

            buffer[current_tail] = element;
            tail.store(next_tail, std::memory_order_release);
            return true;
        }

        // Dequeue element from the buffer, returns false if the buffer is empty
        bool Dequeue(T& element) noexcept {
            size_t current_head = head.load(std::memory_order_relaxed);

            if (current_head == tail.load(std::memory_order_acquire)) {
                // Buffer is empty
                return false;
            }

            element = buffer[current_head];
            head.store((current_head + 1) & mask, std::memory_order_release);
            return true;
        }

        // Returns true if the buffer is full
        inline bool IsFull() const noexcept {
            size_t next_tail = (tail.load(std::memory_order_relaxed) + 1) & mask;
            return next_tail == head.load(std::memory_order_acquire);
        }

        // Returns true if the buffer is empty
        inline bool IsEmpty() const noexcept {
            return head.load(std::memory_order_acquire) == tail.load(std::memory_order_relaxed);
        }

        // Returns the current number of elements in the buffer
        inline size_t Size() const noexcept {
            size_t current_tail = tail.load(std::memory_order_relaxed);
            size_t current_head = head.load(std::memory_order_relaxed);
            return (current_tail - current_head) & mask;
        }
    };
}