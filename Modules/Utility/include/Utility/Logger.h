#pragma once
#include <string>
#include <deque>

namespace Engine::Util {
	class Logger {
		static constexpr uint8_t LOGGER_MAX_SIZE = 10;
	public:
		inline static void Log(std::string&& message) {
			if (logs.size() >= LOGGER_MAX_SIZE) logs.pop_front();
			logs.emplace_back(std::move(message));
		}
		inline static void Log(const std::string& message) {
			if (logs.size() >= LOGGER_MAX_SIZE) logs.pop_front();
			logs.emplace_back(message);
		}
		inline static std::string_view Get_Last_Log() {
			if (logs.empty())
				return "";
			else
				return logs.back();
		}
		inline static std::deque<std::string>& Get_Logs() {
			return logs;
		}
		inline static void Clear() {
			logs.clear();
		}
	private:
		inline static std::deque<std::string> logs;
	};
}