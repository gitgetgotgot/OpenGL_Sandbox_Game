#include <UI/UI_Creator.h>
#include "Core/Menu/CreatorPage.h"

void CreatorPage::update() {

}

void CreatorPage::enable(bool state) {
	canvas->is_enabled = state;
}

void CreatorPage::setup_page() {
	canvas = CoreUI::UI_Creator::create_canvas();
}