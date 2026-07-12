#pragma once

template<class T> class Smart_ptr {
public:
	Smart_ptr() {
		ptr = nullptr;
	}
	Smart_ptr(T* object) {
		ptr = object;
	}
	Smart_ptr(Smart_ptr&& other) noexcept {
		ptr = other.ptr;
		other.ptr = nullptr;
	}
	Smart_ptr& operator=(Smart_ptr&& other) noexcept {
		if (this != &other) {
			delete ptr;
			ptr = other.ptr;
			other.ptr = nullptr;
		}
		return *this;
	}
	~Smart_ptr() {
		if (ptr)
			delete ptr;
	}
	void clear() noexcept {
		delete ptr;
		ptr = nullptr;
	}
	T& operator*() {
		return *ptr;
	}
	T* operator->() {
		return ptr;
	}
	Smart_ptr<T>& operator=(T* object) {
		this->ptr = object;
		return *this;
	}
private:
	T* ptr;
};
