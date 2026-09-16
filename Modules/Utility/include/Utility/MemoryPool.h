#pragma once
#include <vector>
#include <deque>
#include <cstdint>

namespace CoreMemory {
	class MemoryPool {
	public:
		class PoolBucket {
		public:
			PoolBucket() = default;
			~PoolBucket() = default;
			bool Allocate(uint32_t slot_count, uint32_t slot_size);
			void* Get_Free_Memory();
			void Free_Memory(void* ptr);
			void Deallocate();
			void* memory = nullptr;
			std::vector<uint32_t> free_slots;
			uint32_t slot_count = 0;
			uint32_t slot_size = 0;
		};
		template<typename T>
		uint32_t RegisterPoolForClass();
		PoolBucket& GetBucket(uint32_t index);
		bool AllocateBucket(uint32_t slot_count, uint32_t slot_size);
		void Deallocate();
	private:
		MemoryPool() = default;
		~MemoryPool() = default;
		std::deque<PoolBucket> buckets;
	};

	template<typename T>
	inline uint32_t MemoryPool::RegisterPoolForClass() {
		uint32_t aligned_size = (sizeof(T) + 7) & ~(7); //8 bytes aligned size
		uint32_t buckets_size = buckets.size();
		for (uint32_t i = 0; i < buckets_size; i++) {
			if (buckets[i].slot_size == aligned_size) return i;
		}
		AllocateBucket(1000, aligned_size);
		return buckets.size() - 1;
	}

	class MemoryArena {
	public:
		static MemoryArena& Instance() {
			static MemoryArena arena;
			return arena;
		}
		template<typename T>
		MemoryPool& GetMemoryPool() {
			static MemoryPool pool;
			return pool;
		}
	private:
		MemoryArena() = default;
		~MemoryArena() = default;
	};
}
