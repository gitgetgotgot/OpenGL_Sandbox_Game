#include "Core/GameCore.h"
#include "Core/ResourceLoader.h"
#include <UI/UI_System.h>
#include <UI/UI_Creator.h>
#include <thread>

Game::~Game() {
	uninit();
}

bool Game::update() {
	timeMgr.update();

	SystemContext::mouse.get_mouse_ortho_coords(SystemContext::screen);

	if (SystemContext::keyBoard.key_is_pressed(Key::KeyEscape)) {
		return 0;
	}
	if (SystemContext::keyBoard.key_is_pressed(Key::KeyF)) {
		toggle_Fullscreen();
	}

	if (game_update_state == Game_State::inWorld) {
		world->update();
	}
	CoreUI::UI_System::get_instance().update();

	return 1;
}

void Game::render() {
	renderer->clear(1.0, 1.0, 1.0);

	world->render(renderer);
	CoreUI::UI_System::get_instance().render(renderer);

	renderer->present();
}

void Game::input_end_frame() {
	SystemContext::mouse.lb_prev = SystemContext::mouse.lb_curr;
	SystemContext::mouse.rb_prev = SystemContext::mouse.rb_curr;
	SystemContext::mouse.wheel_offset = 0;
	SystemContext::mouse.delta_x = 0;
	SystemContext::mouse.delta_y = 0;
	SystemContext::mouse.overlapped_by_UI_layer = false;

	for (int i = 0; i <= GLFW_KEY_LAST; i++) {
		SystemContext::keyBoard.keyStatesPrev[i] = SystemContext::keyBoard.keyStatesCurr[i];
	}

	SystemContext::keyBoard.currentPressedChars.clear();
}

void Game::toggle_Fullscreen() {
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);

	if (glfwGetWindowMonitor(window) == nullptr) {
		// Switch to full screen
		glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
	}
	else {
		// Switch back to windowed mode with Full HD resolution
		glfwSetWindowMonitor(window, nullptr, 100, 100, 1920, 1080, GLFW_DONT_CARE);
	}
}

void Game::init() {
	srand(time(NULL));

	init_open_gl();
	init_input();

	ResourceLoader::get_instance().Load_Resources();

	CoreUI::UI_System::get_instance().init();

	player.inventory.init();
	for (int i = 2; i < 52; i++) {
		uint16_t amount = rand() % 10000;
		player.inventory.place_item(i, amount);
	}

	player.inventory.update_crafts();
	player.inventory.update_dynamic_craft_slots_items();
	player.inventory.current_player_flags = (uint32_t)CraftCondition::c_WORKBENCH;

	world = std::make_unique<World>();
	world->init(&player);

	auto canvas = CoreUI::UI_Creator::Instance().create_canvas();

	// button test
	auto test_button = CoreUI::UI_Creator::Instance().create_button_image(
		glm::vec2(1.0f, 0.25f),
		glm::vec2(0.0f),
		CoreResource::SpriteManager::get_instance().get_sprite_id("Core:White").value(),
		glm::vec4(0.0f, 0.0f, 0.0f, 1.0f),
		0.1f
	);

	// input field test
	auto test_input_field = CoreUI::UI_Creator::Instance().create_input_field_image9sliced(
		glm::vec2(SystemContext::screen.ratio * 2.0f * 0.99f, 0.15f),
		glm::vec2(0.0f, -0.9f),
		CoreResource::SpriteManager::get_instance().get_sprite9sliced_id("Core:Tooltip").value(),
		glm::vec4(1.0f),
		0.09f
	);

	canvas->add_object(test_button);
	canvas->add_object(test_input_field);

	//openGL settings
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Game::main_loop() {
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		if (!update())
			glfwSetWindowShouldClose(window, true);

		render();

		input_end_frame();
	}
}

void Game::uninit() {
	glfwDestroyWindow(window);
	glfwTerminate();
}

void Game::init_open_gl() {
	glfwInit();
	//version of OpenGL (version 4.6)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	//using the core profile for only modern functions
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(1920, 1080, "Sandbox Game?", NULL, NULL);

	glfwMakeContextCurrent(window);
	glfwSetWindowAspectRatio(window, 16, 9);
	//glfwSwapInterval(0);

	gladLoadGL();

	//openGL version and GPU info
	std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;

	renderer = std::make_unique<OpenGL_Renderer>(window);
}

void Game::init_input() {
	InputHandler::setGLFWwindowCallbacks(window);
}
