#include "Core/Menu/MainPage.h"
#include <Resources/Sprite.h>
#include <UI/UI_Creator.h>
#include <Utility/TimeManager.h>
#include <IOSystem/SystemContext.h>

void MainPage::TooltipLoadRes::update() {
	time_loading += TimeManager::deltaTime;
	if (time_loading >= 0.2f) {
		time_loading = 0.0f;
		++phase;
		if (phase == 4) phase = 0;
		if (phase == 0)		 loading_text->set_text("Loading Content");
		else if (phase == 1) loading_text->set_text("Loading Content.");
		else if (phase == 2) loading_text->set_text("Loading Content..");
		else if (phase == 3) loading_text->set_text("Loading Content...");
	}
}
void MainPage::TooltipLoadWorld::update() {
	time_loading += TimeManager::deltaTime;
	if (time_loading >= 0.2f) {
		time_loading = 0.0f;
		++phase;
		if (phase == 4) phase = 0;
		if (phase == 0)		 loading_text->set_text("Loading World");
		else if (phase == 1) loading_text->set_text("Loading World.");
		else if (phase == 2) loading_text->set_text("Loading World..");
		else if (phase == 3) loading_text->set_text("Loading World...");
	}
}
void MainPage::TooltipGenWorld::update() {
	time_loading += TimeManager::deltaTime;
	if (time_loading >= 0.2f) {
		time_loading = 0.0f;
		++phase;
		if (phase == 4) phase = 0;
		if (phase == 0)		 loading_text->set_text("Generating World");
		else if (phase == 1) loading_text->set_text("Generating World.");
		else if (phase == 2) loading_text->set_text("Generating World..");
		else if (phase == 3) loading_text->set_text("Generating World...");
	}
}

void MainPage::update() {
	text_builder.add_int(TimeManager::Instance().get_current_FPS());
	FPS_text->set_text(text_builder.data());
	text_builder.reset();

	switch (current_tooltip) {
	case MenuTooltip::TOOLTIP_LOAD_RESOURCES:
		tooltip_load_res.update();
		break;
	case MenuTooltip::TOOLTIP_LOAD_WORLD:
		tooltip_load_world.update();
		break;
	case MenuTooltip::TOOLTIP_GENERATE_WORLD:
		tooltip_gen_world.update();
		break;
	}
}

void MainPage::enable(bool state) {
	canvas->is_enabled = state;
}

void MainPage::setup_page() {
	canvas = Engine::UI::UI_Creator::create_canvas();

	uint32_t stone_wall_sprite_id = Engine::Resource::SpriteManager::Instance().get_sprite_id("Core:UI_StoneWall").value();
	uint32_t frame0_id = Engine::Resource::SpriteManager::Instance().get_sprite9sliced_id("Core:UI_Frame0").value();

	auto background = Engine::UI::UI_Creator::create_tiled_image(
		stone_wall_sprite_id, glm::vec2(4.0, 2.0f), glm::vec2(0.0f), glm::vec2(4.0f, 2.0f), glm::vec4(0.7f, 0.7f, 0.7f, 1.0f)
	);
	auto fps_label = Engine::UI::UI_Creator::create_SDF_text(
		glm::vec2(0.05f, 0.05f), glm::vec2(-SystemContext::display.ratio + 0.05f, 0.95f), "fps", glm::vec4(1.0f), 0.05f
	);
	FPS_text = fps_label->get_component<Engine::UI::SDF_Text>();

	// tooltip for loading resources
	tooltip_load_res_obj = Engine::UI::UI_Creator::create_image9sliced(
		frame0_id, glm::vec2(1.5f, 0.5f), glm::vec2(0.0f), glm::vec4(1.0f)
	);
	auto loading_label = Engine::UI::UI_Creator::create_SDF_text(
		glm::vec2(1.4f, 0.1f), glm::vec2(0.0f, 0.0f), "", glm::vec4(1.0f), 0.08f
	);
	tooltip_load_res_obj.add_child(loading_label);
	tooltip_load_res.loading_text = loading_label->get_component<Engine::UI::SDF_Text>();

	// tooltip for generating world
	tooltip_gen_world_obj = Engine::UI::UI_Creator::create_image9sliced(
		frame0_id, glm::vec2(1.5f, 0.5f), glm::vec2(0.0f), glm::vec4(1.0f)
	);
	auto loading_label2 = Engine::UI::UI_Creator::create_SDF_text(
		glm::vec2(1.4f, 0.1f), glm::vec2(0.0f, 0.0f), "", glm::vec4(1.0f), 0.08f
	);
	tooltip_gen_world_obj.add_child(loading_label2);
	tooltip_gen_world.loading_text = loading_label2->get_component<Engine::UI::SDF_Text>();

	// tooltip for loading world
	tooltip_load_world_obj = Engine::UI::UI_Creator::create_image9sliced(
		frame0_id, glm::vec2(1.5f, 0.5f), glm::vec2(0.0f), glm::vec4(1.0f)
	);
	auto loading_label3 = Engine::UI::UI_Creator::create_SDF_text(
		glm::vec2(1.4f, 0.1f), glm::vec2(0.0f, 0.0f), "", glm::vec4(1.0f), 0.08f
	);
	tooltip_load_world_obj.add_child(loading_label3);
	tooltip_load_world.loading_text = loading_label3->get_component<Engine::UI::SDF_Text>();

	canvas->add_object(background);
	canvas->add_object(fps_label);
	canvas->add_object(tooltip_load_res_obj);
	canvas->add_object(tooltip_gen_world_obj);
	canvas->add_object(tooltip_load_world_obj);
	tooltip_load_res_obj->is_enabled = false;
	tooltip_gen_world_obj->is_enabled = false;
	tooltip_load_world_obj->is_enabled = false;
}

void MainPage::show_tooltip_res_load() {
	tooltip_load_res.loading_text->set_text("Loading Content...");
	current_tooltip = MenuTooltip::TOOLTIP_LOAD_RESOURCES;
	tooltip_load_res_obj->is_enabled = true;
}
void MainPage::hide_tooltip_res_load() {
	tooltip_load_res.time_loading = 0.0f;
	current_tooltip = MenuTooltip::TOOLTIP_NONE;
	tooltip_load_res_obj->is_enabled = false;
}

void MainPage::show_tooltip_world_load() {
	tooltip_load_world.loading_text->set_text("Loading World...");
	current_tooltip = MenuTooltip::TOOLTIP_LOAD_WORLD;
	tooltip_load_world_obj->is_enabled = true;
}
void MainPage::hide_tooltip_world_load() {
	tooltip_load_world.time_loading = 0.0f;
	current_tooltip = MenuTooltip::TOOLTIP_NONE;
	tooltip_load_world_obj->is_enabled = false;
}

void MainPage::show_tooltip_world_gen() {
	tooltip_gen_world.loading_text->set_text("Generating World...");
	current_tooltip = MenuTooltip::TOOLTIP_GENERATE_WORLD;
	tooltip_gen_world_obj->is_enabled = true;
}
void MainPage::hide_tooltip_world_gen() {
	tooltip_gen_world.time_loading = 0.0f;
	current_tooltip = MenuTooltip::TOOLTIP_NONE;
	tooltip_gen_world_obj->is_enabled = false;
}