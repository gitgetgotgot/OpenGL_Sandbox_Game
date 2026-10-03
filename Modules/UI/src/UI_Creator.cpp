#include "UI/UI_Creator.h"
#include "UI/CanvasManager.h"
#include "UI/UI_ObjectManager.h"
#include "UI/Image.h"
#include "UI/SDF_Text.h"
#include "UI/InputField.h"
#include "UI/Panel.h"
#include "UI/Button.h"
#include "UI/ScrollView.h"
#include "UI/ScrollBar.h"
#include <Resources/Sprite.h>
#include <IOSystem/SystemContext.h>

CoreUI::UI_Canvas_Ptr CoreUI::UI_Creator::create_canvas() {
	return CanvasManager::get_instance().add();
}

CoreUI::UI_Obj_Ptr CoreUI::UI_Creator::create_ui_empty_object() {
	return UI_ObjectManager::get_instance().add();
}

CoreUI::UI_Obj_Ptr CoreUI::UI_Creator::create_image() {
	UI_Obj_Ptr obj = UI_ObjectManager::get_instance().add();
	obj->add_component<Image>()->Setup_As_Regular();
	return obj;
}

CoreUI::UI_Obj_Ptr CoreUI::UI_Creator::create_image(
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

CoreUI::UI_Obj_Ptr CoreUI::UI_Creator::create_image9sliced() {
	UI_Obj_Ptr obj = UI_ObjectManager::get_instance().add();
	obj->add_component<Image>()->Setup_As_Nine_Sliced();
	return obj;
}

CoreUI::UI_Obj_Ptr CoreUI::UI_Creator::create_image9sliced(
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

CoreUI::UI_Obj_Ptr CoreUI::UI_Creator::create_SDF_text() {
	UI_Obj_Ptr obj = UI_ObjectManager::get_instance().add();
	obj->add_component<SDF_Text>();
	return obj;
}

CoreUI::UI_Obj_Ptr CoreUI::UI_Creator::create_SDF_text(
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

CoreUI::UI_Obj_Ptr CoreUI::UI_Creator::create_panel() {
	UI_Obj_Ptr obj = UI_ObjectManager::get_instance().add();
	obj->add_component<Panel>();
	return obj;
}

CoreUI::UI_Obj_Ptr CoreUI::UI_Creator::create_panel(
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

CoreUI::UI_Obj_Ptr CoreUI::UI_Creator::create_button() {
	UI_Obj_Ptr obj = UI_ObjectManager::get_instance().add();
	obj->add_component<Button>();
	return obj;
}

CoreUI::UI_Obj_Ptr CoreUI::UI_Creator::create_button_image(
	glm::vec2 size, glm::vec2 local_pos, uint32_t sprite_id, std::string txt, glm::vec4 text_color, float text_height
) {
	UI_Obj_Ptr obj = UI_ObjectManager::get_instance().add();
	obj->transform.set_local_pos(local_pos);
	obj->transform.set_size(size);
	auto button = obj->add_component<Button>();
	button->is_interactable = true;

	UI_Obj_Ptr img_obj = UI_ObjectManager::get_instance().add();
	img_obj->transform.set_size(size);
	auto img = img_obj->add_component<Image>();
	img->Setup_As_Regular();
	img->set_sprite(sprite_id);

	UI_Obj_Ptr text_obj = UI_ObjectManager::get_instance().add();
	text_obj->transform.set_size(size);
	auto text = text_obj->add_component<SDF_Text>();
	text->set_text_height(text_height);
	text->set_color(text_color);
	text->set_alignment(SDF_Text::TextHorizAlign::Center_Align, SDF_Text::TextVertAlign::Middle_Align);
	text->set_text(txt);

	obj.add_child(img_obj);
	obj.add_child(text_obj);
	button->bind_components(img, text);

	return obj;
}

CoreUI::UI_Obj_Ptr CoreUI::UI_Creator::create_button_image9sliced(
	glm::vec2 size, glm::vec2 local_pos, uint32_t sprite_id, std::string txt, glm::vec4 text_color, float text_height
) {
	UI_Obj_Ptr obj = UI_ObjectManager::get_instance().add();
	obj->transform.set_local_pos(local_pos);
	obj->transform.set_size(size);
	auto button = obj->add_component<Button>();
	button->is_interactable = true;

	UI_Obj_Ptr img_obj = UI_ObjectManager::get_instance().add();
	img_obj->transform.set_size(size);
	auto img = img_obj->add_component<Image>();
	img->Setup_As_Nine_Sliced();
	img->set_sprite(sprite_id);

	UI_Obj_Ptr text_obj = UI_ObjectManager::get_instance().add();
	text_obj->transform.set_size(size);
	auto text = text_obj->add_component<SDF_Text>();
	text->set_text_height(text_height);
	text->set_color(text_color);
	text->set_alignment(SDF_Text::TextHorizAlign::Center_Align, SDF_Text::TextVertAlign::Middle_Align);
	text->set_text(txt);

	obj.add_child(img_obj);
	obj.add_child(text_obj);
	button->bind_components(img, text);

	return obj;
}

CoreUI::UI_Obj_Ptr CoreUI::UI_Creator::create_input_field() {
	UI_Obj_Ptr obj = UI_ObjectManager::get_instance().add();
	obj->add_component<InputField>();
	return obj;
}

CoreUI::UI_Obj_Ptr CoreUI::UI_Creator::create_input_field_image(
	glm::vec2 size, glm::vec2 local_pos, uint32_t back_sprite_id, glm::vec4 text_color, float text_height
) {
	UI_Obj_Ptr field_obj = UI_ObjectManager::get_instance().add();
	field_obj->transform.set_local_pos(local_pos);
	field_obj->transform.set_size(size);
	auto field = field_obj->add_component<InputField>();
	field->is_interactable = true;

	UI_Obj_Ptr img_obj = UI_ObjectManager::get_instance().add();
	img_obj->transform.set_size(size);
	auto img = img_obj->add_component<Image>();
	img->Setup_As_Regular();
	img->set_sprite(back_sprite_id);

	UI_Obj_Ptr text_obj = UI_ObjectManager::get_instance().add();
	text_obj->transform.set_size(size.x * 0.985f, size.y); //make text bounding box a little bit less on X
	auto field_text = text_obj->add_component<SDF_Text>();
	field_text->set_text_height(text_height);
	field_text->set_color(text_color);

	UI_Obj_Ptr cursor_obj = UI_ObjectManager::get_instance().add();
	cursor_obj->transform.set_size(0.005f, text_height);
	auto cursor_img = cursor_obj->add_component<Image>();
	cursor_img->Setup_As_Regular();
	cursor_img->set_sprite(CoreResource::SpriteManager::get_instance().get_sprite_id("Core:White").value());

	field_obj.add_child(img_obj);
	field_obj.add_child(text_obj);
	field_obj.add_child(cursor_obj);
	field->bind_components(field_text, cursor_img);
	return field_obj;
}
CoreUI::UI_Obj_Ptr CoreUI::UI_Creator::create_input_field_image9sliced(
	glm::vec2 size, glm::vec2 local_pos, uint32_t back_sprite9sliced_id, glm::vec4 text_color, float text_height
) {
	UI_Obj_Ptr field_obj = UI_ObjectManager::get_instance().add();
	field_obj->transform.set_local_pos(local_pos);
	field_obj->transform.set_size(size);
	auto field = field_obj->add_component<InputField>();
	field->is_interactable = true;

	UI_Obj_Ptr img_obj = UI_ObjectManager::get_instance().add();
	img_obj->transform.set_size(size);
	auto img = img_obj->add_component<Image>();
	img->Setup_As_Nine_Sliced();
	img->set_sprite(back_sprite9sliced_id);

	UI_Obj_Ptr text_obj = UI_ObjectManager::get_instance().add();
	text_obj->transform.set_size(size.x * 0.985f, size.y); //make text bounding box a little bit less on X
	auto field_text = text_obj->add_component<SDF_Text>();
	field_text->set_text_height(text_height);
	field_text->set_color(text_color);

	UI_Obj_Ptr cursor_obj = UI_ObjectManager::get_instance().add();
	cursor_obj->transform.set_size(0.005f, text_height);
	auto cursor_img = cursor_obj->add_component<Image>();
	cursor_img->Setup_As_Regular();
	cursor_img->set_sprite(CoreResource::SpriteManager::get_instance().get_sprite_id("Core:White").value());

	field_obj.add_child(img_obj);
	field_obj.add_child(text_obj);
	field_obj.add_child(cursor_obj);
	field->bind_components(field_text, cursor_img);
	return field_obj;
}

CoreUI::UI_Obj_Ptr CoreUI::UI_Creator::create_scroll_view() {
	UI_Obj_Ptr obj = UI_ObjectManager::get_instance().add();
	obj->add_component<ScrollView>();
	return obj;
}

CoreUI::UI_Obj_Ptr CoreUI::UI_Creator::create_scroll_view(glm::vec2 size, glm::vec2 local_pos) {
	UI_Obj_Ptr obj = UI_ObjectManager::get_instance().add();
	obj->transform.set_local_pos(local_pos);
	obj->transform.set_size(size);
	obj->add_component<ScrollView>()->is_interactable = true;
	return obj;
}

CoreUI::UI_Obj_Ptr CoreUI::UI_Creator::create_scroll_bar() {
	UI_Obj_Ptr obj = UI_ObjectManager::get_instance().add();
	obj->add_component<ScrollBar>();
	return obj;
}

CoreUI::UI_Obj_Ptr CoreUI::UI_Creator::create_scroll_bar(
	glm::vec2 scroll_area_size, glm::vec2 track_size, glm::vec2 thumb_size, glm::vec2 local_pos,
	bool track_9_sliced, bool thumb_9_sliced, uint32_t track_sprite_id, uint32_t thumb_sprite_id,
	glm::vec4 track_color, glm::vec4 thumb_color,
	bool vertical
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
	scroll_bar->set_direction(vertical ? ScrollBar::SCROLLBAR_VERTICAL : ScrollBar::SCROLLBAR_HORIZONTAL);
	scroll_bar->bind_components(track_image, thumb_image);
	scroll_bar->set_value(0.0f);

	return obj;
}