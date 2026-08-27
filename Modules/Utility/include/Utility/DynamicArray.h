#pragma once
#include <cstdint>

template<typename T>
struct DynamicArray {
	DynamicArray() {}
	DynamicArray(uint8_t SIZE) {
		data = new T[SIZE];
		size = SIZE;
	}
	~DynamicArray() {
		delete[] data;
	}
	DynamicArray(const DynamicArray&) = delete;
	DynamicArray& operator=(const DynamicArray&) = delete;
	DynamicArray(DynamicArray&& other) noexcept {
		this->data = other.data;
		this->size = other.size;
		other.data = nullptr;
		other.size = 0;
	}
	DynamicArray& operator=(DynamicArray&& other) noexcept {
		if (this != &other) {
			delete[] data;
			this->data = other.data;
			this->size = other.size;
			other.data = nullptr;
			other.size = 0;
		}
		return *this;
	}
	T& operator[](uint8_t index) {
		return data[index];
	}
	T* data = nullptr;
	uint8_t size = 0;
};