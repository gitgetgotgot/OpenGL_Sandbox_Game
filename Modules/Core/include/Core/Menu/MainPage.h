#pragma once
#include <UI/Canvas.h>
#include <UI/SDF_Text.h>
#include <Utility/TextBufferBuilder.h>

class MainPage {
private:
	enum MenuTooltip : uint8_t {
		TOOLTIP_NONE, TOOLTIP_LOAD_RESOURCES, TOOLTIP_LOAD_WORLD, TOOLTIP_GENERATE_WORLD
	};
	struct TooltipLoadRes {
		Engine::UI::UI_Component_Ptr<Engine::UI::SDF_Text> loading_text;
		float time_loading = 0.0f;
		uint32_t phase = 0;
		void update();
	};
	struct TooltipLoadWorld {
		Engine::UI::UI_Component_Ptr<Engine::UI::SDF_Text> loading_text;
		float time_loading = 0.0f;
		uint32_t phase = 0;
		void update();
	};
	struct TooltipGenWorld {
		Engine::UI::UI_Component_Ptr<Engine::UI::SDF_Text> loading_text;
		float time_loading = 0.0f;
		uint32_t phase = 0;
		void update();
	};
public:
	static MainPage& Instance() {
		static MainPage page;
		return page;
	}
	void update();
	void enable(bool state);
	void setup_page();
	void show_tooltip_res_load();
	void hide_tooltip_res_load();
	void show_tooltip_world_load();
	void hide_tooltip_world_load();
	void show_tooltip_world_gen();
	void hide_tooltip_world_gen();
private:
	MainPage() = default;
	~MainPage() = default;
	Engine::UI::UI_Canvas_Ptr canvas;

	Engine::UI::UI_Obj_Ptr tooltip_load_res_obj;
	Engine::UI::UI_Obj_Ptr tooltip_load_world_obj;
	Engine::UI::UI_Obj_Ptr tooltip_gen_world_obj;

	TooltipLoadRes tooltip_load_res;
	TooltipLoadWorld tooltip_load_world;
	TooltipGenWorld tooltip_gen_world;

	Engine::UI::UI_Component_Ptr<Engine::UI::SDF_Text> FPS_text;
	
	MenuTooltip current_tooltip = MenuTooltip::TOOLTIP_NONE;
	TextBufferBuilder text_builder;
};