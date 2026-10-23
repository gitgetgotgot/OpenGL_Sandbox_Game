#pragma once
#include <UI/Canvas.h>

class MainMenuPage {
public:
	static MainMenuPage& Instance() {
		static MainMenuPage page;
		return page;
	}
	void update();
	void enable(bool state);
	void setup_page();
private:
	MainMenuPage() = default;
	~MainMenuPage() = default;
	CoreUI::UI_Canvas_Ptr canvas;
};