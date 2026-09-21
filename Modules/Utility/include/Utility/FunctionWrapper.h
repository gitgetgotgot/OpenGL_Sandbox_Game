#pragma once

class FunctionWrapper {
public:
	FunctionWrapper() = default;
	void set_callback_function(void(*function)()) {
		obj_ptr = reinterpret_cast<void*>(function);
		callback_fun = [](void* p) { reinterpret_cast<void(*)()>(p)(); };
	}
	template<typename Class, void(Class::*method)()>
	void set_callback_method(Class* obj) {
		obj_ptr = static_cast<void*>(obj);
		callback_fun = [](void* p) { (static_cast<Class*>(p)->*method)(); };
	}
	void reset() {
		obj_ptr = nullptr;
		callback_fun = nullptr;
	}
	void operator()() const {
		if (callback_fun && obj_ptr)
			callback_fun(obj_ptr);
	}
private:
	void* obj_ptr = nullptr;
	void(*callback_fun)(void*) = nullptr;
};