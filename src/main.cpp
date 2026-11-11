#include <Core/GameCore.h>
#include <Utility/Logger.h>
#include <print>

int main() {
	GameEngine game;
	try {
		if (game.init() == false) {
			std::print("FATAL ERROR!: {}\n", Engine::Util::Logger::Get_Last_Log());
			return EXIT_FAILURE;
		}
	}
	catch (const std::exception& e) {
		std::println("{}", e.what());
		return EXIT_FAILURE;
	}
	game.main_loop();
}