#include <UI/UI_Creator.h>
#include "Core/Menu/ModsPage.h"
#include <UI/ScrollView.h>
#include "Core/MainMenuManager.h"
#include <UI/Button.h>
#include <Resources/Sprite.h>
#include <Resources/ModsManager.h>

void ModsPage::update() {

}

void ModsPage::enable(bool state) {
	canvas->is_enabled = state;
}

void ModsPage::setup_page() {
	canvas = CoreUI::UI_Creator::create_canvas();

	button_click_sound.setup_source("Core:Button");

	uint32_t gradient2_id = CoreResource::SpriteManager::Instance().get_sprite_id("Core:Gradient2").value();
	uint32_t button64_id = CoreResource::SpriteManager::Instance().get_sprite9sliced_id("Core:UI_Button64p1").value();
	uint32_t frame0_id = CoreResource::SpriteManager::Instance().get_sprite9sliced_id("Core:UI_Frame0").value();

	auto button_back = CoreUI::UI_Creator::create_button_image(
		glm::vec2(1.0f, 0.1f), glm::vec2(0.0f, -0.85f), true, button64_id,
		true, "Back", glm::vec4(0.9f, 0.9f, 0.9f, 1.0f), 0.07f
	);

	button_back->get_component<CoreUI::Button>()->on_button_click.set_callback_method<MainMenuManager, &MainMenuManager::close_mods_page>(&MainMenuManager::Instance());

	//mods background
	auto label_background_obj = CoreUI::UI_Creator::create_image9sliced(
		frame0_id, glm::vec2(1.0f, 0.16f), glm::vec2(0.0f, 0.8f), glm::vec4(1.0f)
	);

	auto mods_label = CoreUI::UI_Creator::create_SDF_text(
		glm::vec2(1.0f, 0.1f), glm::vec2(0.0f), "Mods list", glm::vec4(1.0f), 0.1f
	);
	mods_label->get_component<CoreUI::SDF_Text>()->set_alignment(CoreUI::SDF_Text::TextHorizAlign::Center_Align, CoreUI::SDF_Text::TextVertAlign::Middle_Align);
	label_background_obj.add_child(mods_label);

	auto mods_background_obj = CoreUI::UI_Creator::create_image9sliced(
		frame0_id, glm::vec2(2.12f, 1.52f), glm::vec2(0.0f), glm::vec4(1.0f)
	);
	mods_background_obj->get_component<CoreUI::Image>()->set_border_size(0.06f);

	// scroll_view
	auto mods_scroll_view = CoreUI::UI_Creator::create_scroll_view(
		glm::vec2(2.0f, 1.4f), glm::vec2(0.0f)
	);
	auto scroll_view = mods_scroll_view->get_component<CoreUI::ScrollView>();
	scroll_view->set_group_layout(CoreUI::ScrollView::GroupLayout::LAYOUT_VERTICAL);
	// get content for scroll_view
	mods_content = mods_scroll_view.get_child_by_index(0);
	// vertical scroll_bar for scroll_view
	auto scroll_bar_Y = CoreUI::UI_Creator::create_scroll_bar(
		glm::vec2(0.08f, 1.4f), glm::vec2(0.08f, 1.4f), glm::vec2(0.08f, 0.08f), glm::vec2(0.96f, 0.0f),
		false, true, gradient2_id, button64_id, glm::vec4(0.471f, 0.376f, 0.314f, 1.0f), glm::vec4(1.0f), true, false
	);
	auto scrollbar = scroll_bar_Y->get_component<CoreUI::ScrollBar>();
	scroll_view->has_vertical_scrollbar = true;
	scroll_view->bind_vertical_scrollbar(scrollbar);

	mods_scroll_view.add_child(scroll_bar_Y);

	canvas->add_object(label_background_obj);
	canvas->add_object(mods_background_obj);
	canvas->add_object(button_back);
	canvas->add_object(mods_scroll_view);
}

void ModsPage::load_mods_UI() {
	// prepare sprites IDs
	uint32_t sprite_info_button_id = CoreResource::SpriteManager::Instance().get_sprite_id("Core:UI_ButtonInfo").value();
	uint32_t button32_id = CoreResource::SpriteManager::Instance().get_sprite9sliced_id("Core:UI_Button32p").value();
	uint32_t button64_id = CoreResource::SpriteManager::Instance().get_sprite9sliced_id("Core:UI_Button64p1").value();
	uint32_t icon_frame_id = CoreResource::SpriteManager::Instance().get_sprite_id("Core:UI_IconFrame").value();
	uint32_t icon_id = CoreResource::SpriteManager::Instance().get_sprite_id("Core:Gradient").value();

	for (auto& mod : ModsManager::get_instance().get_mods()) {
		// create UI mod element
		auto mod_element = CoreUI::UI_Creator::create_image9sliced(button32_id, glm::vec2(1.9f, 0.27f), glm::vec2(0.0f), glm::vec4(0.441, 0.379, 0.332, 1.0f));
		auto button_toggle = CoreUI::UI_Creator::create_button_image(
			glm::vec2(0.4f, 0.1f), glm::vec2(-0.45f, -0.06f), true, button64_id,
			true, "Disabled", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), 0.07f
		);
		auto button_info = CoreUI::UI_Creator::create_button_image(glm::vec2(0.1f, 0.1f), glm::vec2(-0.18f, -0.06f), false, sprite_info_button_id);

		std::string mod_name_version(mod.UID); mod_name_version += " (", mod_name_version += mod.version, mod_name_version += ")";
		auto mod_name = CoreUI::UI_Creator::create_SDF_text(glm::vec2(1.5f, 0.1f), glm::vec2(0.1f, 0.06f), mod_name_version, glm::vec4(1.0f), 0.07f);

		auto mod_icon_frame = CoreUI::UI_Creator::create_image(icon_frame_id, glm::vec2(0.22f), glm::vec2(-0.8f, 0.0f), glm::vec4(1.0f));
		auto mod_icon = CoreUI::UI_Creator::create_image(icon_id, glm::vec2(0.165f), glm::vec2(0.0f), glm::vec4(1.0f));
		mod_icon_frame.add_child(mod_icon);

		mod_element.add_child(mod_icon_frame);
		mod_element.add_child(mod_name);
		mod_element.add_child(button_toggle);
		mod_element.add_child(button_info);

		// add element to scroll_view content
		mods_content.add_child(mod_element);
	}
}