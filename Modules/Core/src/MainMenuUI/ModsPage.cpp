#include "Core/MainMenuUI/ModsPage.h"
#include "Core/MainMenuUI/MenuUIController.h"
#include <UI/UI_Creator.h>
#include <UI/ScrollView.h>
#include <UI/Button.h>
#include <Resources/Sprite.h>
#include <Resources/ModsManager.h>

void ModElement::_toggle_mod() {
	button_click_sound.play();
	Engine::Mods::ModInfo& mod_info = Engine::Mods::ModsManager::Instance().get_mod(mod_id);
	mod_info.enabled = !mod_info.enabled;
	if (mod_info.enabled) {
		toggle_button_text->set_text("Enabled");
		toggle_button_text->set_color(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	}
	else {
		toggle_button_text->set_text("Disabled");
		toggle_button_text->set_color(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	}
}

void ModElement::_show_mod_info() {
	button_click_sound.play();
	Engine::Mods::ModInfo& mod_info = Engine::Mods::ModsManager::Instance().get_mod(mod_id);
	ModsPage::Instance().show_mod_info(mod_info.description);
}

void ModsPage::update() {

}

void ModsPage::enable(bool state) {
	canvas->is_enabled = state;
}

void ModsPage::setup_page() {
	canvas = Engine::UI::UI_Creator::create_canvas();

	ModElement::button_click_sound.setup_source("Core:Button");
	button_click_sound.setup_source("Core:Button");

	uint32_t gradient2_id = Engine::Resource::SpriteManager::Instance().get_sprite_id("Core:Gradient2").value();
	uint32_t button64_id = Engine::Resource::SpriteManager::Instance().get_sprite9sliced_id("Core:UI_Button64p1").value();
	uint32_t frame0_id = Engine::Resource::SpriteManager::Instance().get_sprite9sliced_id("Core:UI_Frame0").value();

	// 1. Mods List page
	{
		auto button_back = Engine::UI::UI_Creator::create_button_image(
			glm::vec2(1.0f, 0.1f), glm::vec2(0.0f, -0.85f), true, button64_id,
			true, "Back", glm::vec4(0.9f, 0.9f, 0.9f, 1.0f), 0.07f
		);
		button_back->get_component<Engine::UI::Button>()->on_button_click.set_callback_method<MenuUIController, &MenuUIController::close_mods_page>(&MenuUIController::Instance());

		//mods background
		auto label_background_obj = Engine::UI::UI_Creator::create_image9sliced(
			frame0_id, glm::vec2(1.0f, 0.16f), glm::vec2(0.0f, 0.8f), glm::vec4(1.0f)
		);
		auto mods_label = Engine::UI::UI_Creator::create_SDF_text(
			glm::vec2(1.0f, 0.1f), glm::vec2(0.0f), "Mods list", glm::vec4(1.0f), 0.075f
		);
		mods_label->get_component<Engine::UI::SDF_Text>()->set_alignment(Engine::UI::SDF_Text::TextHorizAlign::Center_Align, Engine::UI::SDF_Text::TextVertAlign::Middle_Align);
		label_background_obj.add_child(mods_label);

		auto mods_background_obj = Engine::UI::UI_Creator::create_image9sliced(
			frame0_id, glm::vec2(2.12f, 1.52f), glm::vec2(0.0f), glm::vec4(1.0f)
		);
		mods_background_obj->get_component<Engine::UI::Image>()->set_border_size(0.06f);

		// scroll_view
		auto mods_scroll_view = Engine::UI::UI_Creator::create_scroll_view(
			glm::vec2(2.0f, 1.4f), glm::vec2(0.0f)
		);
		auto scroll_view = mods_scroll_view->get_component<Engine::UI::ScrollView>();
		scroll_view->set_group_layout(Engine::UI::ScrollView::GroupLayout::LAYOUT_VERTICAL);
		mods_view = scroll_view;
		// get content for scroll_view
		mods_content = mods_scroll_view.get_child_by_index(0);

		// vertical scroll_bar for scroll_view
		auto scroll_bar_Y = Engine::UI::UI_Creator::create_scroll_bar(
			glm::vec2(0.08f, 1.4f), glm::vec2(0.08f, 1.4f), glm::vec2(0.08f, 0.08f), glm::vec2(0.96f, 0.0f),
			false, true, gradient2_id, button64_id, glm::vec4(0.471f, 0.376f, 0.314f, 1.0f), glm::vec4(1.0f), true, false
		);
		auto scrollbar = scroll_bar_Y->get_component<Engine::UI::ScrollBar>();
		scroll_view->has_vertical_scrollbar = true;
		scroll_view->bind_vertical_scrollbar(scrollbar);

		mods_scroll_view.add_child(scroll_bar_Y);

		mods_list_obj = Engine::UI::UI_Creator::create_ui_empty_object();
		mods_list_obj.add_child(label_background_obj);
		mods_list_obj.add_child(mods_background_obj);
		mods_list_obj.add_child(button_back);
		mods_list_obj.add_child(mods_scroll_view);
	}
	// 2. Mod Description page
	{
		auto button_back = Engine::UI::UI_Creator::create_button_image(
			glm::vec2(1.0f, 0.1f), glm::vec2(0.0f, -0.85f), true, button64_id,
			true, "Back", glm::vec4(0.9f, 0.9f, 0.9f, 1.0f), 0.07f
		);
		button_back->get_component<Engine::UI::Button>()->on_button_click.set_callback_method<ModsPage, &ModsPage::hide_mod_info>(&ModsPage::Instance());

		//mod description background
		auto label_background_obj = Engine::UI::UI_Creator::create_image9sliced(
			frame0_id, glm::vec2(1.0f, 0.16f), glm::vec2(0.0f, 0.8f), glm::vec4(1.0f)
		);

		auto mod_desc_label = Engine::UI::UI_Creator::create_SDF_text(
			glm::vec2(1.0f, 0.1f), glm::vec2(0.0f), "Mod Description", glm::vec4(1.0f), 0.075f
		);
		mod_desc_label->get_component<Engine::UI::SDF_Text>()->set_alignment(Engine::UI::SDF_Text::TextHorizAlign::Center_Align, Engine::UI::SDF_Text::TextVertAlign::Middle_Align);
		label_background_obj.add_child(mod_desc_label);

		auto desc_background_obj = Engine::UI::UI_Creator::create_image9sliced(
			frame0_id, glm::vec2(2.12f, 1.52f), glm::vec2(0.0f), glm::vec4(1.0f)
		);
		desc_background_obj->get_component<Engine::UI::Image>()->set_border_size(0.06f);

		// scroll_view
		auto desc_scroll_view = Engine::UI::UI_Creator::create_scroll_view(
			glm::vec2(2.0f, 1.4f), glm::vec2(0.0f)
		);
		auto scroll_view = desc_scroll_view->get_component<Engine::UI::ScrollView>();
		scroll_view->set_group_layout(Engine::UI::ScrollView::GroupLayout::LAYOUT_VERTICAL);
		mod_description_view = scroll_view;
		// get content for scroll_view
		auto desc_content = desc_scroll_view.get_child_by_index(0);

		// vertical scroll_bar for scroll_view
		auto scroll_bar_Y = Engine::UI::UI_Creator::create_scroll_bar(
			glm::vec2(0.08f, 1.4f), glm::vec2(0.08f, 1.4f), glm::vec2(0.08f, 0.08f), glm::vec2(0.96f, 0.0f),
			false, true, gradient2_id, button64_id, glm::vec4(0.471f, 0.376f, 0.314f, 1.0f), glm::vec4(1.0f), true, false
		);
		auto scrollbar = scroll_bar_Y->get_component<Engine::UI::ScrollBar>();
		scroll_view->has_vertical_scrollbar = true;
		scroll_view->bind_vertical_scrollbar(scrollbar);

		desc_scroll_view.add_child(scroll_bar_Y);

		mod_description_obj = Engine::UI::UI_Creator::create_ui_empty_object();
		mod_description_obj.add_child(label_background_obj);
		mod_description_obj.add_child(desc_background_obj);
		mod_description_obj.add_child(button_back);
		mod_description_obj.add_child(desc_scroll_view);

		// add description text in content
		auto mod_description_obj = Engine::UI::UI_Creator::create_SDF_text(
			glm::vec2(1.9f, 0.1f), glm::vec2(0.0f), "", glm::vec4(1.0f), 0.05f
		);
		mod_description = mod_description_obj->get_component<Engine::UI::SDF_Text>();
		mod_description->set_alignment(Engine::UI::SDF_Text::TextHorizAlign::Left_Align, Engine::UI::SDF_Text::TextVertAlign::Top_Align);
		mod_description->adaptive_widget_height = true;
		desc_content.add_child(mod_description_obj);
	}

	canvas->add_object(mods_list_obj);
	canvas->add_object(mod_description_obj);
	mod_description_obj->is_enabled = false;
	mods_list_obj->is_enabled = true;
}

void ModsPage::load_mods_UI() {
	// prepare sprites IDs
	uint32_t sprite_info_button_id = Engine::Resource::SpriteManager::Instance().get_sprite_id("Core:UI_ButtonInfo").value();
	uint32_t button32_id = Engine::Resource::SpriteManager::Instance().get_sprite9sliced_id("Core:UI_Button32p").value();
	uint32_t button64_id = Engine::Resource::SpriteManager::Instance().get_sprite9sliced_id("Core:UI_Button64p1").value();
	uint32_t icon_frame_id = Engine::Resource::SpriteManager::Instance().get_sprite_id("Core:UI_IconFrame").value();
	glm::vec4 color_enabled(0.0f, 1.0f, 0.0f, 1.0f);
	glm::vec4 color_disabled(1.0f, 0.0f, 0.0f, 1.0f);

	uint32_t mod_ID = 0;
	for (auto& mod : Engine::Mods::ModsManager::Instance().get_mods()) {
		std::string mod_icon_sprite_UID(mod.UID);
		mod_icon_sprite_UID += ":mod_icon";
		uint32_t icon_id = Engine::Resource::SpriteManager::Instance().get_sprite_id(mod_icon_sprite_UID).value();

		// create UI mod element
		auto mod_element = Engine::UI::UI_Creator::create_image9sliced(button32_id, glm::vec2(1.9f, 0.27f), glm::vec2(0.0f), glm::vec4(0.441, 0.379, 0.332, 1.0f));
		auto button_toggle = Engine::UI::UI_Creator::create_button_image(
			glm::vec2(0.4f, 0.1f), glm::vec2(-0.45f, -0.06f), true, button64_id,
			true, mod.enabled ? "Enabled" : "Disabled", mod.enabled ? color_enabled : color_disabled, 0.07f
		);
		auto button_info = Engine::UI::UI_Creator::create_button_image(glm::vec2(0.1f, 0.1f), glm::vec2(-0.18f, -0.06f), false, sprite_info_button_id);

		std::string mod_name_version(mod.UID); mod_name_version += " (", mod_name_version += mod.version, mod_name_version += ")";
		auto mod_name = Engine::UI::UI_Creator::create_SDF_text(glm::vec2(1.5f, 0.1f), glm::vec2(0.1f, 0.06f), mod_name_version, glm::vec4(1.0f), 0.07f);

		auto mod_icon_frame = Engine::UI::UI_Creator::create_image(icon_frame_id, glm::vec2(0.22f), glm::vec2(-0.8f, 0.0f), glm::vec4(1.0f));
		auto mod_icon = Engine::UI::UI_Creator::create_image(icon_id, glm::vec2(0.165f), glm::vec2(0.0f), glm::vec4(1.0f));
		mod_icon_frame.add_child(mod_icon);

		mod_element.add_child(mod_icon_frame);
		mod_element.add_child(mod_name);
		mod_element.add_child(button_toggle);
		mod_element.add_child(button_info);

		// add element to scroll_view content
		mods_content.add_child(mod_element);

		// add mod element
		ModElement& new_mod_element = mod_elements.emplace_back();
		new_mod_element.mod_id = mod_ID++;
		auto toggle_button_comp = button_toggle->get_component<Engine::UI::Button>();
		auto info_button_comp = button_info->get_component<Engine::UI::Button>();
		new_mod_element.toggle_button_text = toggle_button_comp->button_text;
		toggle_button_comp->on_button_click.set_callback_method<ModElement, &ModElement::_toggle_mod>(&new_mod_element);
		info_button_comp->on_button_click.set_callback_method<ModElement, &ModElement::_show_mod_info>(&new_mod_element);
	}
	mods_view->force_update_content();
}

void ModsPage::show_mod_info(std::string& mod_info_txt) {
	mod_description->set_text(mod_info_txt);
	mod_description->force_update();
	mod_description_view->force_update_content();
	mod_description_obj->is_enabled = true;
	mods_list_obj->is_enabled = false;
}

void ModsPage::hide_mod_info() {
	button_click_sound.play();
	mod_description_obj->is_enabled = false;
	mods_list_obj->is_enabled = true;
}