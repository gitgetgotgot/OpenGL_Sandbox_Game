#include <UI/UI_Creator.h>
#include "Core/Menu/ModsPage.h"

void ModsPage::update() {

}

void ModsPage::enable(bool state) {
	canvas->is_enabled = state;
}

void ModsPage::setup_page() {
	canvas = CoreUI::UI_Creator::create_canvas();
}