#pragma once
#include <glm/glm.hpp>
#include <chrono>

constexpr double PI = 3.14159265358979323846264338327950288;
constexpr float rainbow_speed = 2.f;
constexpr float rainbow_period = 2.f * PI;

class TimeManager {
public:
	TimeManager();
	~TimeManager();
	void update();
	void update_world_time();
	float get_rainbow_time();
	float get_global_light_value();
	int get_current_FPS();
	glm::vec3& get_sky_color();
	glm::vec4 getRainbowColor();
	void toggle_dayTime();
	inline static float deltaTime = 0.0f;
	inline static float deltaTimeScale = 1.0f;
private:
	glm::vec3 skyColor{}; //current color of the sky
	glm::vec3 dayColor{ 0.53, 0.81, 0.92 };
	glm::vec3 nightColor{ 0.0, 0.0, 0.05 };
	float day_ratio = 1.0; //1 is day, 0 is night 
	const float dayTime = 600.f; //10 minutes is the duration of day/night
	float cycle_time = 0.0; //time since the day/night has started
	bool isDay = true; //true - day, false - night
	float day_time_speed = 1.0f; //delta time speed multiplier

	float game_delta_Time = 0.f;
	int FPS_counter = 0;
	int current_FPS = 0;
	float rainbow_color_time = 0;

	std::chrono::steady_clock::time_point previous_time;
	std::chrono::steady_clock::time_point current_time;
};