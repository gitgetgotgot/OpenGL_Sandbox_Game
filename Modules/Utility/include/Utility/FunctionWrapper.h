#pragma once

template<typename ... Args>
class FunctionWrapper {
public:
	FunctionWrapper() = default;
	void set_callback_function(void(*function)(Args...)) {
		obj_ptr = reinterpret_cast<void*>(function);
		callback_fun = [](void* p, Args... args) {
			reinterpret_cast<void(*)(Args...)>(p)(std::forward<Args>(args)...);
		};
	}
	template<typename Class, void(Class::*method)(Args...)>
	void set_callback_method(Class* obj) {
		obj_ptr = static_cast<void*>(obj);
		callback_fun = [](void* p, Args... args) {
			(static_cast<Class*>(p)->*method)(std::forward<Args>(args)...);
		};
	}
	void reset() {
		obj_ptr = nullptr;
		callback_fun = nullptr;
	}
	void operator()(Args... args) const {
		if (callback_fun && obj_ptr)
			callback_fun(obj_ptr, std::forward<Args>(args)...);
	}
private:
	void* obj_ptr = nullptr;
	void(*callback_fun)(void*, Args...) = nullptr;
};