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

static void update_array_default(float* array, size_t count, float multiplier) {
	for (size_t i = 0; i < count; i++) {
		array[i] *= multiplier;
	}
}

#include <immintrin.h>
#include <thread>
static void print_simd_support() {
	int info[4];

	__cpuid(info, 1);
	bool hasAVX = (info[2] & (1 << 28)) != 0;

	__cpuid(info, 7);
	bool hasAVX2 = (info[1] & (1 << 5)) != 0;
	bool hasAVX512 = (info[1] & (1 << 16)) != 0;

	std::cout << "AVX: " << hasAVX << "\n";
	std::cout << "AVX2: " << hasAVX2 << "\n";
	std::cout << "AVX512: " << hasAVX512 << "\n";
}
static void update_array_AVX256(float* array, size_t count, float multiplier) {
	size_t i = 0;
	size_t avx_count = count & ~size_t(7);

	__m256 factor = _mm256_set1_ps(multiplier);
	for (; i < avx_count; i += 8) {
		__m256 v = _mm256_loadu_ps(&array[i]);
		v = _mm256_mul_ps(v, factor);
		_mm256_storeu_ps(&array[i], v);
	}
	for (; i < count; i++) {
		array[i] *= multiplier;
	}
}
static void update_array_AVX512(float* array, size_t count, float multiplier) {
	size_t i = 0;
	size_t avx_count = count & ~size_t(15);

	__m512 factor = _mm512_set1_ps(multiplier);
	for (; i < avx_count; i += 16) {
		__m512 v = _mm512_loadu_ps(&array[i]);
		v = _mm512_mul_ps(v, factor);
		_mm512_storeu_ps(&array[i], v);
	}
	for (; i < count; i++) {
		array[i] *= multiplier;
	}
}
static void update_array_AVX2_range(float* array, size_t begin, size_t end, float multiplier) {
	size_t i = begin;
	size_t avx_end = begin + ((end - begin) & ~size_t(7));

	__m256 factor = _mm256_set1_ps(multiplier);

	for (; i < avx_end; i += 8) {
		__m256 v = _mm256_loadu_ps(&array[i]);
		v = _mm256_mul_ps(v, factor);
		_mm256_storeu_ps(&array[i], v);
	}

	for (; i < end; i++) {
		array[i] *= multiplier;
	}
}
static void update_array_AVX2_mt(float* array, size_t count, float multiplier, unsigned threads) {
	std::vector<std::thread> workers;
	size_t chunk = count / threads;

	for (unsigned t = 0; t < threads; t++) {
		size_t begin = t * chunk;
		size_t end = (t == threads - 1 ? count : begin + chunk);

		workers.emplace_back(update_array_AVX2_range, array, begin, end, multiplier);
	}

	for (auto& th : workers) th.join();
}

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
	try {
		game.init();
	}
	catch (const std::exception& e) {
		std::cerr << "[INIT]: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	game.main_loop();
}