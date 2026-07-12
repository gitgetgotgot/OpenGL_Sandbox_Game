#include <Utility/TimeManager.h>

TimeManager::TimeManager() {
	previous_time = std::chrono::steady_clock::now();
}

TimeManager::~TimeManager() {

}

void TimeManager::update() {
	//delta time
	current_time = std::chrono::steady_clock::now();
	deltaTime = std::chrono::duration<float>(current_time - previous_time).count();
	previous_time = current_time;

	//fps
	game_delta_Time += deltaTime;
	FPS_counter++;
	if (game_delta_Time >= 1.0) {
		game_delta_Time = 0.f;
		current_FPS = FPS_counter;
		FPS_counter = 0;
	}

	deltaTime *= deltaTimeScale;
	//rainbow color time
	rainbow_color_time = std::fmod(rainbow_color_time + rainbow_speed * deltaTime, rainbow_period);
}

void TimeManager::update_world_time() {
	//check if day/night ended
	if (cycle_time >= dayTime) {
		cycle_time = 0.0;
		isDay = !isDay;
	}
	//check if it's starting of day/night
	if (cycle_time <= dayTime * 0.1) {
		if (isDay) {
			day_ratio = 0.5 * (1 + cycle_time / (dayTime * 0.1));
		}
		else {
			day_ratio = 0.5 * (1 - cycle_time / (dayTime * 0.1));
		}
		skyColor = nightColor * (1.0f - day_ratio) + dayColor * day_ratio;
	}
	//check if it's ending of day/night
	else if (cycle_time >= dayTime * 0.9) {
		if (isDay) {
			day_ratio = 0.5 * (1 + (dayTime - cycle_time) / (dayTime * 0.1));
		}
		else {
			day_ratio = 0.5 * (1 - (dayTime - cycle_time) / (dayTime * 0.1));
		}
		skyColor = nightColor * (1.0f - day_ratio) + dayColor * day_ratio;
	}
	//if it's a day/night time
	else {
		if (isDay) {
			day_ratio = 1.0;
			skyColor = dayColor;
		}
		else {
			day_ratio = 0.0;
			skyColor = nightColor;
		}
	}
	cycle_time += deltaTime;
}

float TimeManager::get_rainbow_time() {
	return rainbow_color_time;
}

float TimeManager::get_global_light_value() {
	return day_ratio;
}

int TimeManager::get_current_FPS() {
	return current_FPS;
}

glm::vec3& TimeManager::get_sky_color() {
	return skyColor;
}

glm::vec4 TimeManager::getRainbowColor() {
	float r = std::sin(rainbow_color_time + 0.0f) * 0.5f + 0.5f;
	float g = std::sin(rainbow_color_time + 2.094f) * 0.5f + 0.5f; // 120° = 2π/3
	float b = std::sin(rainbow_color_time + 4.188f) * 0.5f + 0.5f; // 240° = 4π/3
	return glm::vec4(r, g, b, 1.f);
}

void TimeManager::toggle_dayTime() {
	isDay = !isDay;
}
