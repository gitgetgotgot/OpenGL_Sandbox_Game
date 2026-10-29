#include "UI/UI_Creator.h"
#include "UI/CanvasManager.h"
#include "UI/UI_ObjectManager.h"
#include "UI/Image.h"
#include "UI/TiledImage.h"
#include "UI/SDF_Text.h"
#include "UI/InputField.h"
#include "UI/Panel.h"
#include "UI/Button.h"
#include "UI/ScrollView.h"
#include "UI/ScrollViewContent.h"
#include "UI/ScrollBar.h"
#include <Resources/Sprite.h>
#include <IOSystem/SystemContext.h>

Engine::UI::UI_Canvas_Ptr Engine::UI::UI_Creator::create_canvas() {
	return CanvasManager::get_instance().add();
}

Engine::UI::UI_Obj_Ptr Engine::UI::UI_Creator::create_ui_empty_object() {
	return UI_ObjectManager::get_instance().add();
}

Engine::UI::UI_Obj_Ptr Engine::UI::UI_Creator::create_image() {
	UI_Obj_Ptr obj = UI_ObjectManager::get_instance().add();
	obj->add_component<Image>()->Setup_As_Regular();
	return obj;
}

Engine::UI::UI_Obj_Ptr Engine::UI::UI_Creator::create_image(
	uint32_t sprite_id, glm::vec2 size, glm::vec2 local_pos, glm::vec4 color
) {
	UI_Obj_Ptr obj = UI_ObjectManager::get_instance().add();
	obj->transform.set_local_pos(local_pos);
	obj->transform.set_size(size);
	auto img = obj->add_component<Image>();
	img->Setup_As_Regular();
	img->set_sprite(sprite_id);
	img->set_color(color);
	return obj;
}

Engine::UI::UI_Obj_Ptr Engine::UI::UI_Creator::create_image9sliced() {
	UI_Obj_Ptr obj = UI_ObjectManager::get_instance().add();
	obj->add_component<Image>()->Setup_As_Nine_Sliced();
	return obj;
}

Engine::UI::UI_Obj_Ptr Engine::UI::UI_Creator::create_image9sliced(
	uint32_t sprite9sliced_id, glm::vec2 size, glm::vec2 local_pos, glm::vec4 color
) {
	UI_Obj_Ptr obj = UI_ObjectManager::get_instance().add();
	obj->transform.set_local_pos(local_pos);
	obj->transform.set_size(size);
	auto img = obj->add_component<Image>();
	img->Setup_As_Nine_Sliced();
	img->set_sprite(sprite9sliced_id);
	img->set_color(color);
	return obj;
}

Engine::UI::UI_Obj_Ptr Engine::UI::UI_Creator::create_tiled_image() {
	UI_Obj_Ptr obj = UI_ObjectManager::get_instance().add();
	obj->add_component<TiledImage>();
	return obj;
}

Engine::UI::UI_Obj_Ptr Engine::UI::UI_Creator::create_tiled_image(
	uint32_t sprite_id, glm::vec2 size, glm::vec2 local_pos, glm::vec2 tiles_count, glm::vec4 color
) {
	UI_Obj_Ptr obj = UI_ObjectManager::get_instance().add();
	obj->transform.set_local_pos(local_pos);
	obj->transform.set_size(size);
	auto img = obj->add_component<TiledImage>();
	img->set_sprite(sprite_id);
	img->set_tile(tiles_count.x, tiles_count.y);
	img->set_color(color);
	return obj;
}

Engine::UI::UI_Obj_Ptr Engine::UI::UI_Creator::create_SDF_text() {
	UI_Obj_Ptr obj = UI_ObjectManager::get_instance().add();
	obj->add_component<SDF_Text>();
	return obj;
}

Engine::UI::UI_Obj_Ptr Engine::UI::UI_Creator::create_SDF_text(
	glm::vec2 size, glm::vec2 local_pos, std::string text, glm::vec4 text_color, float text_height
) {
	UI_Obj_Ptr obj = UI_ObjectManager::get_instance().add();
	obj->transform.set_local_pos(local_pos);
	obj->transform.set_size(size);
	auto obj_text = obj->add_component<SDF_Text>();
	obj_text->set_text(text);
	obj_text->set_color(text_color);
	obj_text->set_text_height(text_height);
	return obj;
}

Engine::UI::UI_Obj_Ptr Engine::UI::UI_Creator::create_panel() {
	UI_Obj_Ptr obj = UI_ObjectManager::get_instance().add();
	obj->add_component<Panel>();
	return obj;
}

Engine::UI::UI_Obj_Ptr Engine::UI::UI_Creator::create_panel(
	glm::vec2 size, glm::vec2 local_pos, uint32_t sprite_id, glm::vec4 color
) {
	UI_Obj_Ptr obj = UI_ObjectManager::get_instance().add();
	obj->transform.set_local_pos(local_pos);
	obj->transform.set_size(size);
	auto panel = obj->add_component<Panel>();
	panel->set_sprite(sprite_id);
	panel->set_color(color);
	return obj;
}

Engine::UI::UI_Obj_Ptr Engine::UI::UI_Creator::create_button() {
	UI_Obj_Ptr obj = UI_ObjectManager::get_instance().add();
	obj->add_component<Button>();
	return obj;
}

Engine::UI::UI_Obj_Ptr Engine::UI::UI_Creator::create_button_image(
	glm::vec2 size, glm::vec2 local_pos, bool image_is_9_sliced, uint32_t sprite_id, bool has_text,
	std::string txt, glm::vec4 text_color, float text_height
) {
	UI_Obj_Ptr obj = UI_ObjectManager::get_instance().add();
	obj->transform.set_local_pos(local_pos);
	obj->transform.set_size(size);
	auto button = obj->add_component<Button>();
	button->is_interactable = true;
	button->blocks_input = true;

	UI_Obj_Ptr img_obj = UI_ObjectManager::get_instance().add();
	img_obj->transform.set_size(size);
	auto img = img_obj->add_component<Image>();
	if (image_is_9_sliced)
		img->Setup_As_Nine_Sliced();
	else
		img->Setup_As_Regular();
	img->set_sprite(sprite_id);

	obj.add_child(img_obj);

	UI_Component_Ptr<SDF_Text> text;
	if (has_text) {
		UI_Obj_Ptr text_obj = UI_ObjectManager::get_instance().add();
		text_obj->transform.set_size(size);
		text = text_obj->add_component<SDF_Text>();
		text->set_text_height(text_height);
		text->set_color(text_color);
		text->set_alignment(SDF_Text::TextHorizAlign::Center_Align, SDF_Text::TextVertAlign::Middle_Align);
		text->set_text(txt);
		obj.add_child(text_obj);
	}
	button->bind_components(img, text); //if has no text then empty text is binded

	return obj;
}

Engine::UI::UI_Obj_Ptr Engine::UI::UI_Creator::create_input_field() {
	UI_Obj_Ptr obj = UI_ObjectManager::get_instance().add();
	obj->add_component<InputField>();
	return obj;
}

Engine::UI::UI_Obj_Ptr Engine::UI::UI_Creator::create_input_field_image(
	glm::vec2 size, glm::vec2 local_pos, bool image_is_9_sliced, uint32_t back_sprite_id, glm::vec4 text_color, float text_height, float border_size, float sliced_img_border_size
) {
	UI_Obj_Ptr field_obj = UI_ObjectManager::get_instance().add();
	field_obj->transform.set_local_pos(local_pos);
	field_obj->transform.set_size(size);
	auto field = field_obj->add_component<InputField>();
	field->is_interactable = true;
	field->blocks_input = true;

	UI_Obj_Ptr img_obj = UI_ObjectManager::get_instance().add();
	img_obj->transform.set_size(size);
	auto img = img_obj->add_component<Image>();
	if (image_is_9_sliced) {
		img->Setup_As_Nine_Sliced();
		img->set_border_size(sliced_img_border_size);
	}
	else
		img->Setup_As_Regular();
	img->set_sprite(back_sprite_id);

	glm::vec2 text_inner_area_size(size.x - border_size * 2.0f, size.y - border_size * 2.0f);

	UI_Obj_Ptr text_obj = UI_ObjectManager::get_instance().add();
	text_obj->transform.set_size(text_inner_area_size);
	auto field_text = text_obj->add_component<SDF_Text>();
	field_text->set_text_height(text_height);
	field_text->set_color(text_color);

	UI_Obj_Ptr cursor_obj = UI_ObjectManager::get_instance().add();
	cursor_obj->transform.set_size(text_height * 0.05f, text_height);
	auto cursor_img = cursor_obj->add_component<Image>();
	cursor_img->Setup_As_Regular();
	cursor_img->set_sprite(Engine::Resource::SpriteManager::Instance().get_sprite_id("Core:White").value());

	field_obj.add_child(img_obj);
	field_obj.add_child(text_obj);
	field_obj.add_child(cursor_obj);
	field->bind_components(img, field_text, cursor_img);
	return field_obj;
}

Engine::UI::UI_Obj_Ptr Engine::UI::UI_Creator::create_scroll_view() {
	UI_Obj_Ptr obj = UI_ObjectManager::get_instance().add();
	obj->add_component<ScrollView>();
	return obj;
}

Engine::UI::UI_Obj_Ptr Engine::UI::UI_Creator::create_scroll_view(glm::vec2 size, glm::vec2 local_pos) {
	UI_Obj_Ptr obj = UI_ObjectManager::get_instance().add();
	obj->transform.set_local_pos(local_pos);
	obj->transform.set_size(size);
	auto scroll_view = obj->add_component<ScrollView>();
	
	UI_Obj_Ptr content_obj = UI_ObjectManager::get_instance().add();
	content_obj->transform.set_size(size);
	auto scroll_view_content = content_obj->add_component<ScrollViewContent>();
	obj.add_child(content_obj);

	scroll_view->is_interactable = true;
	scroll_view->blocks_input = true;
	scroll_view->bind_content(scroll_view_content);
	return obj;
}

Engine::UI::UI_Obj_Ptr Engine::UI::UI_Creator::create_scroll_bar() {
	UI_Obj_Ptr obj = UI_ObjectManager::get_instance().add();
	obj->add_component<ScrollBar>();
	return obj;
}

Engine::UI::UI_Obj_Ptr Engine::UI::UI_Creator::create_scroll_bar(
	glm::vec2 scroll_area_size, glm::vec2 track_size, glm::vec2 thumb_size, glm::vec2 local_pos,
	bool track_9_sliced, bool thumb_9_sliced, uint32_t track_sprite_id, uint32_t thumb_sprite_id,
	glm::vec4 track_color, glm::vec4 thumb_color,
	bool vertical, bool slider_mode, bool is_discrete, uint16_t discrete_steps
) {
	UI_Obj_Ptr obj = UI_ObjectManager::get_instance().add();
	obj->transform.set_size(scroll_area_size);
	obj->transform.set_local_pos(local_pos);

	auto scroll_bar = obj->add_component<ScrollBar>();
	UI_Obj_Ptr track_obj = UI_ObjectManager::get_instance().add();
	track_obj->transform.set_size(track_size);
	auto track_image = track_obj->add_component<Image>();
	if (track_9_sliced)
		track_image->Setup_As_Nine_Sliced();
	else
		track_image->Setup_As_Regular();
	track_image->set_sprite(track_sprite_id);
	track_image->set_color(track_color);

	UI_Obj_Ptr thumb_obj = UI_ObjectManager::get_instance().add();
	thumb_obj->transform.set_size(thumb_size);
	auto thumb_image = thumb_obj->add_component<Image>();
	if (thumb_9_sliced)
		thumb_image->Setup_As_Nine_Sliced();
	else
		thumb_image->Setup_As_Regular();
	thumb_image->set_sprite(thumb_sprite_id);
	thumb_image->set_color(thumb_color);

	obj.add_child(track_obj);
	obj.add_child(thumb_obj);

	scroll_bar->is_interactable = true;
	scroll_bar->blocks_input = true;
	scroll_bar->set_direction(vertical ? ScrollBar::SCROLLBAR_VERTICAL : ScrollBar::SCROLLBAR_HORIZONTAL);
	scroll_bar->set_scroll_mode(slider_mode ? ScrollBar::SLIDER_MODE : ScrollBar::PAGEABLE_MODE);
	scroll_bar->bind_components(track_image, thumb_image);
	scroll_bar->set_value(0.0f);
	scroll_bar->is_discrete = is_discrete;
	scroll_bar->set_steps_amount(discrete_steps);

	return obj;
}