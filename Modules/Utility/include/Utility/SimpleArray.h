#pragma once
#include <cstdint>
#include <cstddef>

namespace Engine::Util {
	//simple and compact dynamic array that has constant size
	template<typename T>
	struct SimpleArray {
		SimpleArray() {}
		SimpleArray(uint8_t SIZE) {
			data = new T[SIZE];
			size = SIZE;
		}
		~SimpleArray() {
			delete[] data;
		}
		SimpleArray(const SimpleArray&) = delete;
		SimpleArray& operator=(const SimpleArray&) = delete;
		SimpleArray(SimpleArray&& other) noexcept {
			this->data = other.data;
			this->size = other.size;
			other.data = nullptr;
			other.size = 0;
		}
		SimpleArray& operator=(SimpleArray&& other) noexcept {
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
}