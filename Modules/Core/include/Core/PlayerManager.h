#pragma once

class PlayerManager {
public:
	static PlayerManager& Instance() {
		static PlayerManager mgr;
		return mgr;
	}
private:
	PlayerManager() = default;
	~PlayerManager() = default;
};