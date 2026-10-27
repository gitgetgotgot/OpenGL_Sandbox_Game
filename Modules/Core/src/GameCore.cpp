#include "Core/GameCore.h"
#include "Core/ResourceLoader.h"
#include "Core/MainMenuManager.h"
#include "Core/SettingsManager.h"
#include "Core/WorldManager.h"
#include <UI/UI_System.h>
#include <Audio/AudioSystem.h>
#include <Resources/TextureArrayMgr.h>
#include <Utility/TimeManager.h>
#include <iostream>

GameEngine::~GameEngine() {
	uninit();
}

bool GameEngine::update() {
	TimeManager::Instance().update();

	SystemContext::mouse.get_mouse_ortho_coords(SystemContext::display);

	if (SystemContext::should_exit()) return 0;

	(this->*update_proc)();

	return 1;
}

void GameEngine::render() {
	renderer->clear(0.0, 0.0, 0.0);

	Engine::Resource::TextureArrayManager::Instance().bind_texture_arrays();

	if (game_render_state == Game_State::inWorld) {
		WorldManager::Instance().render(renderer);
	}

	Engine::UI::UI_System::Instance().render(renderer);

	renderer->present();
}

void GameEngine::input_end_frame() {
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
	Engine::Entity::EntitySystem::Instance().uninit();

	WorldManager::Instance().clear();

	glfwDestroyWindow(window);
	glfwTerminate();
}

void GameEngine::switch_update_state(Game_State state) {
	game_update_state = state;
}

void GameEngine::init_MainWindow() {
	glfwInit();

	// OpenGL Core 4.6
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

	// using the core profile for only modern functions
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// create main window and hide before minimum game data is loaded
	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

	window = glfwCreateWindow(1920, 1080, "2D Sandbox", NULL, NULL);

	glfwMakeContextCurrent(window);
	glfwSetWindowAspectRatio(window, 16, 9);
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
	SystemContext::display.window = window;
	SystemContext::display.setup_monitor_video_modes();
}

void GameEngine::init_main_resources() {
	Engine::Resource::TextureArrayManager::Instance().init();

	Engine::Audio::AudioSystem::Instance().init();
	Engine::Audio::AudioSystem::Instance().add_sound_group("Core:UI");
	Engine::Audio::AudioSystem::Instance().add_sound_group("Core:Music");
	Engine::Audio::AudioSystem::Instance().add_sound_group("Core:SFX");

	Engine::UI::UI_System::Instance().init();

	Engine::Entity::EntitySystem::Instance().init();

	ResourceLoader::Instance().load_minimal_core_resources();
	SettingsManager::Instance().load_settings();
	MainMenuManager::Instance().init();

	glfwShowWindow(window);

	update_proc = &GameEngine::loading_update;
	ResourceLoader::Instance().start_resources_loading();
	MainMenuManager::Instance().start_loading_resources();

	/*
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
	*/
}

void GameEngine::loading_update() {
	// lock mutex for resources loader when updating systems
	ResourceLoader::Instance().lock_resources_mutex();
	MainMenuManager::Instance().update();
	Engine::UI::UI_System::Instance().update();
	Engine::Audio::AudioSystem::Instance().update();
	ResourceLoader::Instance().unlock_resources_mutex();

	// load availale resources from queue in main thread (textures)
	ResourceLoader::Instance().load_resources_from_queue();

	// change update proc if resources are loaded
	if (ResourceLoader::Instance().resources_loading_ready()) {
		ResourceLoader::Instance().stop_resources_loading();
		MainMenuManager::Instance().stop_loading_resources();
		MainMenuManager::Instance().open_main_menu_page();
		MainMenuManager::Instance().load_mods();
		game_update_state = Game_State::inMainMenu;
		update_proc = &GameEngine::gameplay_update;
	}
}

void GameEngine::gameplay_update() {
	switch (game_update_state) {
	case Game_State::inMainMenu: {
		MainMenuManager::Instance().update();
		game_render_state = Game_State::inMainMenu;
		break;
	}
	case Game_State::inWorld: {
		WorldManager::Instance().update();
		game_render_state = Game_State::inWorld;
		break;
	}
	}

	Engine::UI::UI_System::Instance().update();
	Engine::Audio::AudioSystem::Instance().update();
}