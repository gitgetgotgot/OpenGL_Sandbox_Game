#pragma once
#include <UI/Canvas.h>
#include <UI/SDF_Text.h>
#include <Utility/TextBufferBuilder.h>

class MainPage {
public:
	static MainPage& Instance() {
		static MainPage page;
		return page;
	}
	void update();
	void enable(bool state);
	void setup_page();
	void start_loading_resources();
	void stop_loading_resources();
private:
	MainPage() = default;
	~MainPage() = default;
	CoreUI::UI_Canvas_Ptr canvas;
	CoreUI::UI_Component_Ptr<CoreUI::SDF_Text> loading_text;
	CoreUI::UI_Component_Ptr<CoreUI::SDF_Text> FPS_text;
	bool is_loading = false;
	float time_loading = 0.0f;
	uint32_t phase = 0;
	TextBufferBuilder text_builder;
};