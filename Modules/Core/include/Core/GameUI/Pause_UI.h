#pragma once
#include <UI/Canvas.h>

class Pause_UI {
public:
	static Pause_UI& Instance() {
		static Pause_UI p;
		return p;
	}
	void update();
	void enable(bool state);
	void setup();
private:
	Pause_UI() = default;
	~Pause_UI() = default;

	Engine::UI::UI_Canvas_Ptr canvas;
};