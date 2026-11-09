#include <Core/GameCore.h>
#include <print>

int main() {
	GameEngine game;
	try {
		if (game.init() == false) {
			return EXIT_FAILURE;
		}
	}
	catch (const std::exception& e) {
		std::println("{}", e.what());
		return EXIT_FAILURE;
	}
	game.main_loop();
}