#pragma once
#include <UI/Canvas.h>

class MainMenuPage {
public:
	void update();
	void enable(bool state);
	void setup_page();
private:
	CoreUI::UI_Canvas_Ptr canvas;
};