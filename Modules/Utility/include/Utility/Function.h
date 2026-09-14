#pragma once
#include <cstdint>
#include <numeric>
#include <random>
#include <algorithm>
#include <iostream>
#include <chrono>

struct VTable {
	void (*update)(void*);
	void (*destroy)(void*);
};

struct EntryHandle {
	void* obj_ptr;
	VTable* vt;
};

template<typename Derived>
class Base {
public:
	static VTable* get_VTable() {
		static VTable vt {
			[](void* p) { static_cast<Derived*>(p)->update_impl(); },
			[](void* p) { static_cast<Derived*>(p)->~Derived(); }
		};
		return &vt;
	}
	void update() {
		static_cast<Derived*>(this)->update_impl();
	}
};
class DerivedA : public Base<DerivedA> {
public:
	~DerivedA() {}
	void update_impl();
	uint64_t data;
};

class DerivedB : public Base<DerivedB> {
public:
	~DerivedB() {}
	void update_impl();
	uint64_t data;
};

class Base2 {
public:
	virtual ~Base2() = default;
	virtual void update() {
		volatile int x = 0;
		x += 9;
	}
};
class Derived2A : public Base2 {
public:
	void update() override;
};
class Derived2B : public Base2 {
public:
	void update() override;
};

class GG {
public:
	
};

#include <Utility/MemoryPool.h>

inline void blabla() {
	std::chrono::high_resolution_clock::time_point start;
	std::chrono::high_resolution_clock::time_point end;

	Base2** v = new Base2 * [1'000'000];
	EntryHandle* c = new EntryHandle [1'000'000];

	CoreMemory::MemoryPool::get_instance().AllocateBucket(1'000'000, 8);

	for (int i = 0; i < 1'000'000; i++) {
		if (i % 2 == 0) {
			//virtual
			v[i] = new Derived2A();
			//crtp + vt
			void* mem = CoreMemory::MemoryPool::get_instance().GetBucket(0).Get_Free_Memory();
			c[i].obj_ptr = new(mem) DerivedA();
			c[i].vt = DerivedA::get_VTable();
		}
		else {
			//virtual
			v[i] = new Derived2B();
			//crtp + vt
			void* mem = CoreMemory::MemoryPool::get_instance().GetBucket(0).Get_Free_Memory();
			c[i].obj_ptr = new(mem) DerivedB();
			c[i].vt = DerivedB::get_VTable();
		}
	}

	std::vector<int> indices(1'000'000);
	std::iota(indices.begin(), indices.end(), 0);
	std::shuffle(indices.begin(), indices.end(), std::mt19937{ std::random_device{}() });

	start = std::chrono::high_resolution_clock::now();
	for (auto& i : indices) {
		v[i]->update();
	}
	end = std::chrono::high_resolution_clock::now();
	std::cout << "Virtual Time = " << std::chrono::duration<float>(end - start).count() << "s." << std::endl;

	start = std::chrono::high_resolution_clock::now();
	for (auto& i : indices) {
		c[i].vt->update(c[i].obj_ptr);
	}
	end = std::chrono::high_resolution_clock::now();
	std::cout << "CRTP VTable Time = " << std::chrono::duration<float>(end - start).count() << "s." << std::endl;

	for (int i = 0; i < 1'000'000; i++) {
		delete v[i];
		c[i].vt->destroy(c[i].obj_ptr);
	}
	delete[] v;
	delete[] c;

	CoreMemory::MemoryPool::get_instance().Deallocate();
}