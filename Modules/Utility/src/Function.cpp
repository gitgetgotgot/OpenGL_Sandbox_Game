#include <Utility/Function.h>

void DerivedA::update_impl() {
	volatile int x = 100;
	x += 9;
}
void DerivedB::update_impl() {
	volatile int x = 100;
	x -= 9;
}


void Derived2A::update() {
	volatile int x = 100;
	x += 9;
}
void Derived2B::update() {
	volatile int x = 100;
	x -= 9;
}