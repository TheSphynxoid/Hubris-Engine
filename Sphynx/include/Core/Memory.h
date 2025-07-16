#pragma once
#include <exception>
#include <atomic>
#include <memory>
#include <cassert>
#include <map>

namespace Sphynx::Core {
	/**
	 * @brief Handle<T> is a non-owning pointer to an engine-managed T. The engine controls the lifetime of T. Do not delete.
	 */
	template <typename T>
	struct Handle {
	private:
		static_assert(std::is_pointer_v(T) == false);
		static_assert(std::is_void_v(T) == false);
		T* ptr;
	public:
		explicit Handle(T* p = nullptr) : ptr(p) {}
		Handle(const Handle&) = delete;
		Handle& operator=(const Handle&) = delete;

		T* operator->() const { return ptr; }
		T& operator*() const { return *ptr; }
		T* get() const { return ptr; }
		operator bool() const { return ptr != nullptr; }

	};


	struct RegionConfig {
		bool AllowResize : 1;
		bool JustFit : 1;
		bool EnsureZeroBeforeFree : 1;
		bool FixedSizeExpansion : 1;
		bool MetadataSector : 4;
		size_t RegionSize;
		size_t ResizeFactor = 16 * sizeof(char);
	};
	class Memory {
	private:
		static inline thread_local bool RegionActive = false;
		static inline thread_local void* RegionBuffer = nullptr;
		static inline thread_local void* FreePointer;
		static inline thread_local std::multimap<size_t, void*> Freeblocks;
		static inline thread_local RegionConfig Config;
		static inline thread_local size_t UsedSpace = 0;
		////100-byte temporary store.
		//static inline thread_local void* TempStore = calloc(1, 100 * sizeof(char));
		//static inline thread_local size_t TempSize = 100 * sizeof(char);
		//static inline thread_local bool IsTempUsed = false;
	public:

		static void StartRegion(RegionConfig& config) {
			
			if (RegionActive) {
				throw std::exception("Attempted to start a memory region inside a memory region, or never free a previous region.");
			}
			RegionActive = true;
			Config = config;
			RegionBuffer = calloc(1, Config.RegionSize);
		}
		static void* RegionAllocate(size_t size, size_t align = 0) {
			if (size + UsedSpace >= Config.RegionSize) {
				if (!Config.AllowResize) {
					throw std::exception("Out of Region memory.");
				}
				ResizeRegion(size + Config.RegionSize + Config.ResizeFactor);
			}
			//Found no empty block with the specified size and the backing buffer has available space.
			if (Freeblocks.find(size) == Freeblocks.cend()) {
				////Defragment
				//if (IsTempUsed) {
				//	//block until defrag is complete
				//}
				//else {
				//	//Store in temp and start defrag as a defered task.
				//	if (TempSize < size + align) {
				//		void* reallocated = realloc(TempStore, TempSize + size + align);
				//		if (reallocated == nullptr) {
				//			//realloc failed
				//			throw std::exception("Could not expand temporary store, engine terminated");
				//		}
				//		TempStore = reallocated;
				//	}
				//	return TempStore;
				//}
			}
		}
		static void ResizeRegion(size_t newsize) {

		}
		static void RegionFree() {
			if (RegionActive) {
				free(RegionBuffer);
				RegionBuffer = nullptr;
				RegionActive = false;
				UsedSpace = 0;
			}
		}
	};
}