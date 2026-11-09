#include "Core/GameCore.h"
#include "Core/ResourceLoader.h"
#include "Core/SettingsManager.h"
#include "Core/World/WorldManager.h"
#include "Core/MainMenuUI/MenuUIController.h"
#include "Core/GameUI/GameUIController.h"

#include <UI/UI_System.h>
#include <Audio/AudioSystem.h>
#include <Resources/TextureArrayMgr.h>
#include <DeviceSystem/DeviceContext.h>
#include <Utility/TimeManager.h>
#include <Utility/Logger.h>
#include <print>

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

	renderer->end_frame();
	DeviceContext::present_end_frame();
}

bool GameEngine::init() {
	srand(time(NULL));
	bool ok = true;
	if (ok) ok = init_MainWindow();
	if (ok) ok = init_GraphicsAPI();
	if (ok) ok = init_input();
	if (ok) ok = init_main_resources();
	return ok;
}

void GameEngine::main_loop() {
	while (!DeviceContext::main_window_should_close()) {
		DeviceContext::poll_events();

		if (!update()) DeviceContext::close_main_window();

		render();

		DeviceContext::process_end_input();
	}
}

void GameEngine::uninit() {
	Engine::Resource::TextureArrayManager::Instance().uninit();
	Engine::Audio::AudioSystem::Instance().uninit();
	Engine::UI::UI_System::Instance().uninit();
	Engine::Core::EntitySystem::Instance().uninit();

	WorldManager::Instance().clear();

	DeviceContext::destroy_main_window();
}

void GameEngine::switch_update_state(Game_State state) {
	game_update_state = state;
}

bool GameEngine::init_MainWindow() {
	return DeviceContext::init_main_window();
}

bool GameEngine::init_GraphicsAPI() {
	renderer = std::make_unique<Engine::Graphics::OpenGL_Renderer>();
	Engine::Graphics::OpenGL_Renderer::renderer_ptr = renderer.get();
	return renderer->init();
}

bool GameEngine::init_input() {
	bool ok = true;
	DeviceContext::setup_GLFW_callbacks();
	ok = DeviceContext::display.setup_monitor_video_modes();
	return ok;
}

bool GameEngine::init_main_resources() {
	Engine::Resource::TextureArrayManager::Instance().init();

	Engine::Audio::AudioSystem::Instance().init();
	Engine::Audio::AudioSystem::Instance().add_sound_group("Core:UI");
	Engine::Audio::AudioSystem::Instance().add_sound_group("Core:Music");
	Engine::Audio::AudioSystem::Instance().add_sound_group("Core:SFX");

	SettingsManager::Instance().load_settings();

	Engine::UI::UI_System::Instance().init();

	Engine::Core::EntitySystem::Instance().init();

	if (ResourceLoader::Instance().load_minimal_core_resources() == false) {
		DeviceContext::exit_application();
		std::println("{}", Engine::Util::Logger::Get_Last_Log_Ref());
		return false;
	}

	MenuUIController::Instance().init();
	GameUIController::Instance().init();

	DeviceContext::toggle_main_window(true);

	update_proc = &GameEngine::loading_update;
	ResourceLoader::Instance().start_resources_loading();
	MenuUIController::Instance().start_loading_resources();

	return true;
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
			std::println("{}", Engine::Util::Logger::Get_Last_Log_Ref());
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