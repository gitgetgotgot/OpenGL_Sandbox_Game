#include "Utility/MemoryPool.h"

bool CoreMemory::MemoryPool::PoolBucket::Allocate(uint32_t slot_count, uint32_t slot_size) {
	this->slot_count = slot_count;
	this->slot_size = slot_size;
	free_slots.resize(slot_count);
	for (uint32_t i = 0; i < slot_count; i++) free_slots[i] = i;
	memory = malloc(static_cast<size_t>(slot_count) * slot_size);
	return memory != nullptr;
}

void* CoreMemory::MemoryPool::PoolBucket::Get_Free_Memory() {
	if (free_slots.empty()) return nullptr;
	uint32_t free_slot_index = free_slots.back();
	free_slots.pop_back();
	void* free_memory = static_cast<uint8_t*>(memory) + (static_cast<size_t>(slot_size) * free_slot_index);
	return free_memory;
}

void CoreMemory::MemoryPool::PoolBucket::Free_Memory(void* ptr) {
	if (!ptr) return;
	size_t byte_offset = static_cast<uint8_t*>(ptr) - static_cast<uint8_t*>(memory);
	uint32_t slot_index = static_cast<uint32_t>(byte_offset / slot_size);
	free_slots.push_back(slot_index);
}

void CoreMemory::MemoryPool::PoolBucket::Deallocate() {
	free(memory);
}


CoreMemory::MemoryPool::PoolBucket& CoreMemory::MemoryPool::GetBucket(uint32_t index) {
	return buckets[index];
}

bool CoreMemory::MemoryPool::AllocateBucket(uint32_t slot_count, uint32_t slot_size) {
	return buckets.emplace_back().Allocate(slot_count, slot_size);
}

void CoreMemory::MemoryPool::Deallocate() {
	for (auto& b : buckets) b.Deallocate();
}