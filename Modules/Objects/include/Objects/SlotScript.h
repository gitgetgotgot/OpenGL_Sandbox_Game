#pragma once
#include <UI/UI_Behaviour.h>
#include <UI/Image.h>
#include <UI/UI_Object.h>

class CustomImageBehaviour : public CoreUI::UI_Behaviour<CustomImageBehaviour> {
public:
	CustomImageBehaviour() = default;
	~CustomImageBehaviour() = default;

	void OnPointerEnter() {
		image->set_sprite(51);
	}
	//void OnPointerHover() {}
	void OnPointerExit() {
		image->set_sprite(50);
	}
	void OnPointerDown() {
		left = !left;
		image_object->transform.set_local_pos(left ? -1.0f : 1.0f, 0.0f);
	}
	//void OnPointerHeld() {}
	//void OnPointerUp() {}

	CoreUI::UI_Component_Ptr<CoreUI::Image> image;
	CoreUI::UI_Obj_Ptr image_object;
	bool left = true;
};