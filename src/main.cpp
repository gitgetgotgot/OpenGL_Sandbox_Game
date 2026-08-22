#include <Core/GameCore.h>

#include "Speaker.h"
#include <Windows.h>

//glfwSwapInterval(0); - remove FPS limiter

// VAO - vertex array object(to use different VBOs), VBO - vertex buffer object(for vertices), EBO - element buffer object (for indices)
// VAO first, then VBO, then EBO!
// Unbind in the same way

// local -> world (model matrix)
// world -> view (view matrix)
// view -> clip (projection matrix)
// clip -> screen
// Vclip = Mprojection * Mview * Mmodel * Vlocal


int main() {
	typedef SpeakerFactory* (*GET_SPEAKER_FACTORY)();
	HMODULE lib = LoadLibraryA("library_test.dll");
	if (!lib) {
		std::cout << "Error: " << GetLastError() << std::endl;
		return 1;
	}
	GET_SPEAKER_FACTORY factory = (GET_SPEAKER_FACTORY)GetProcAddress(lib, "GET_SPEAKER_FACTORY");
	
	SpeakerFactory* core_factory = new CoreSpeakerFactory();
	SpeakerFactory* modded_factory = factory();
	
	BaseSpeaker* core_speaker = core_factory->create_speaker();
	BaseSpeaker* modded_speaker = modded_factory->create_speaker();

	core_speaker->speak();
	modded_speaker->speak();

	Game game;
	game.init();
	game.main_loop();
	game.uninit();
}