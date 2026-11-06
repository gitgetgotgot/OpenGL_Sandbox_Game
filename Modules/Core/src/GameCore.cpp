#include "Core/GameCore.h"
#include "Core/ResourceLoader.h"
#include "Core/SettingsManager.h"
#include "Core/World/WorldManager.h"
#include "Core/MainMenuUI/MenuUIController.h"
#include "Core/GameUI/GameUIController.h"

#include <UI/UI_System.h>
#include <Audio/AudioSystem.h>
#include <Resources/TextureArrayMgr.h>
#include <Utility/TimeManager.h>
#include <Utility/Logger.h>
#include <iostream>

static void GLFWErrorCallback(int error_code, const char* description) {
	std::cerr << "GLFW Error [" << error_code << "]: " << description << std::endl;
}

GameEngine::~GameEngine() {
	uninit();
}

bool GameEngine::update() {
	Engine::Util::TimeManager::Instance().update();

	DeviceContext::mouse.get_mouse_ortho_coords(DeviceContext::display);

	if (DeviceContext::should_exit()) return 0;

	(this->*update_proc)();

	return 1;
}

void GameEngine::render() {
	renderer->clear(0.0, 0.0, 0.0);

	Engine::Resource::TextureArrayManager::Instance().bind_texture_arrays();

	if (game_render_state == Game_State::inGame) {
		WorldManager::Instance().render(renderer);
	}

	Engine::UI::UI_System::Instance().render(renderer);

	renderer->present();
}

void GameEngine::input_end_frame() {
	DeviceContext::mouse.lb_prev = DeviceContext::mouse.lb_curr;
	DeviceContext::mouse.rb_prev = DeviceContext::mouse.rb_curr;
	DeviceContext::mouse.wheel_offset = 0;
	DeviceContext::mouse.delta_x = 0;
	DeviceContext::mouse.delta_y = 0;
	DeviceContext::mouse.blocked_by_UI = false;

	for (int i = 0; i <= GLFW_KEY_LAST; i++) {
		DeviceContext::keyBoard.keyStatesPrev[i] = DeviceContext::keyBoard.keyStatesCurr[i];
	}

	DeviceContext::keyBoard.currentPressedChars.clear();
}

void GameEngine::init() {
	srand(time(NULL));

	init_MainWindow();
	init_OpenGL();
	init_input();
	init_main_resources();
}

void GameEngine::main_loop() {
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		if (!update())
			glfwSetWindowShouldClose(window, true);

		render();

		input_end_frame();
	}
}

void GameEngine::uninit() {
	Engine::Resource::TextureArrayManager::Instance().uninit();
	Engine::Audio::AudioSystem::Instance().uninit();
	Engine::UI::UI_System::Instance().uninit();
	Engine::Core::EntitySystem::Instance().uninit();

	WorldManager::Instance().clear();

	glfwDestroyWindow(window);
	glfwTerminate();
}

void GameEngine::switch_update_state(Game_State state) {
	game_update_state = state;
}

void GameEngine::init_MainWindow() {
	glfwSetErrorCallback(GLFWErrorCallback);
	if (!glfwInit()) {
		throw std::runtime_error("Failed to initialize GLFW");
	}

	// OpenGL Core 4.6
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

	// using the core profile for only modern functions
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// create main window and hide before minimum game data is loaded
	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

	window = glfwCreateWindow(1920, 1080, "2D Sandbox", NULL, NULL);
	if (!window) {
		glfwTerminate();
		throw std::runtime_error("Failed to create GLFW window");
	}

	glfwMakeContextCurrent(window);
	glfwSetWindowAspectRatio(window, 16, 9);

	//int fb_width, fb_height;
	//glfwGetFramebufferSize(window, &fb_width, &fb_height);
	//glViewport(0, 0, fb_width, fb_height);
	//DeviceContext::display.update_resolution(fb_width, fb_height);
}

void GameEngine::init_OpenGL() {
	int version = gladLoadGL((GLADloadfunc)glfwGetProcAddress);
	if (!version)
		throw std::runtime_error("Failed to initialize GLAD for OpenGL!");
	std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;

	// setup main renderer with OpenGL
	renderer = std::make_unique<Engine::Graphics::OpenGL_Renderer>(window);

	// OpenGL base settings
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void GameEngine::init_input() {
	InputHandler::setGLFWwindowCallbacks(window);
	DeviceContext::display.window = window;
	DeviceContext::display.setup_monitor_video_modes();
}

void GameEngine::init_main_resources() {
	Engine::Resource::TextureArrayManager::Instance().init();

	Engine::Audio::AudioSystem::Instance().init();
	Engine::Audio::AudioSystem::Instance().add_sound_group("Core:UI");
	Engine::Audio::AudioSystem::Instance().add_sound_group("Core:Music");
	Engine::Audio::AudioSystem::Instance().add_sound_group("Core:SFX");

	Engine::UI::UI_System::Instance().init();

	Engine::Core::EntitySystem::Instance().init();

	if (ResourceLoader::Instance().load_minimal_core_resources() == false) {
		DeviceContext::exit_application();
		std::cout << Engine::Util::Logger::Get_Last_Log() << std::endl;
	}
	SettingsManager::Instance().load_settings();
	MenuUIController::Instance().init();
	GameUIController::Instance().init();

	glfwShowWindow(window);

	update_proc = &GameEngine::loading_update;
	ResourceLoader::Instance().start_resources_loading();
	MenuUIController::Instance().start_loading_resources();

	/*
	player.inventory.init();
	for (int i = 2; i < 52; i++) {
		uint16_t amount = rand() % 10000;
		player.inventory.place_item(i, amount);
	}
	player.inventory.update_crafts();
	player.inventory.update_dynamic_craft_slots_items();
	*/
}

void GameEngine::loading_update() {
	// lock mutex for resources loader when updating systems
	ResourceLoader::Instance().lock_resources_mutex();
	MenuUIController::Instance().update();
	Engine::UI::UI_System::Instance().update();
	Engine::Audio::AudioSystem::Instance().update();
	ResourceLoader::Instance().unlock_resources_mutex();

	// load availale resources from queue in main thread (textures)
	ResourceLoader::Instance().load_resources_from_queue();

	// change update proc if resources are loaded
	if (ResourceLoader::Instance().get_loading_status() != ResourceLoader::LOAD_NOT_READY) {
		ResourceLoader::Instance().stop_resources_loading();
		MenuUIController::Instance().stop_loading_resources();
		MenuUIController::Instance().open_main_menu_page();
		MenuUIController::Instance().load_mods();
		game_update_state = Game_State::inMainMenu;
		update_proc = &GameEngine::gameplay_update;
		if (ResourceLoader::Instance().get_loading_status() == ResourceLoader::LOAD_FAILED) {
			DeviceContext::exit_application();
			std::cout << Engine::Util::Logger::Get_Last_Log() << std::endl;
		}
	}
}

void GameEngine::gameplay_update() {
	Engine::UI::UI_System::Instance().update();
	Engine::Audio::AudioSystem::Instance().update();

	switch (game_update_state) {
	case Game_State::inMainMenu: {
		MenuUIController::Instance().update();
		game_render_state = Game_State::inMainMenu;
		break;
	}
	case Game_State::inGame: {
		GameUIController::Instance().update();
		WorldManager::Instance().update();
		game_render_state = Game_State::inGame;
		break;
	}
	}
}