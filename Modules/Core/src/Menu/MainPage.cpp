#include "Core/Menu/MainPage.h"
#include <Resources/Sprite.h>
#include <UI/UI_Creator.h>
#include <Utility/TimeManager.h>
#include <IOSystem/SystemContext.h>

void MainPage::update() {
	text_builder.add_int(TimeManager::Instance().get_current_FPS());
	FPS_text->set_text(text_builder.data());
	text_builder.reset();

	if (is_loading) {
		time_loading += TimeManager::deltaTime;
		if (time_loading >= 0.2f) {
			time_loading = 0.0f;
			++phase;
			if (phase == 4) phase = 0;
			if (phase == 0)		 loading_text->set_text("Loading Data");
			else if (phase == 1) loading_text->set_text("Loading Data.");
			else if (phase == 2) loading_text->set_text("Loading Data..");
			else if (phase == 3) loading_text->set_text("Loading Data...");
		}
	}
}

void MainPage::enable(bool state) {
	canvas->is_enabled = state;
}

void MainPage::setup_page() {
	canvas = CoreUI::UI_Creator::create_canvas();

	uint32_t stone_wall_sprite_id = CoreResource::SpriteManager::Instance().get_sprite_id("Core:UI_StoneWall").value();

	auto background = CoreUI::UI_Creator::create_tiled_image(
		stone_wall_sprite_id, glm::vec2(4.0, 2.0f), glm::vec2(0.0f), glm::vec2(4.0f, 2.0f), glm::vec4(0.7f, 0.7f, 0.7f, 1.0f)
	);

	auto loading_label = CoreUI::UI_Creator::create_SDF_text(
		glm::vec2(1.5f, 0.1f), glm::vec2(0.0f, -0.8f), "", glm::vec4(1.0f), 0.15f
	);
	loading_text = loading_label->get_component<CoreUI::SDF_Text>();

	auto fps_label = CoreUI::UI_Creator::create_SDF_text(
		glm::vec2(0.05f, 0.05f), glm::vec2(-SystemContext::display.ratio + 0.05f, 0.95f), "fps", glm::vec4(1.0f), 0.05f
	);
	FPS_text = fps_label->get_component<CoreUI::SDF_Text>();

	canvas->add_object(background);
	canvas->add_object(fps_label);
	canvas->add_object(loading_label);

	start_loading_resources();
}

void MainPage::start_loading_resources() {
	loading_text->set_text("Loading Data...");
	is_loading = true;
}

void MainPage::stop_loading_resources() {
	loading_text->set_text("");
	is_loading = false;
	time_loading = 0.0f;
}