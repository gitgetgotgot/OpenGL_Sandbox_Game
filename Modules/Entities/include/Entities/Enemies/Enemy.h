#pragma once
#include <Entities/GameEntities.h>
#include <vector>

class EnemyInfo : public EntityInfo {
public:
	EnemyInfo(EnemyType type, std::string name, int HP, int dmg, int defense, glm::vec2 hitboxSize, glm::vec2 spriteSize) :
		EntityInfo(isEnemy, name, hitboxSize, spriteSize), type{ type }, HP{ HP }, damage{ dmg }, defense{ defense } {
	}
	int get_HP() const override { return HP; }
	int get_dmg() const override { return damage; }
	int get_defense() const override { return defense; }
	EnemyType get_enemy_type() override { return type; }
	std::vector<DropInfo>& get_drop_v() override { return drop_items; }
private:
	int HP, damage, defense;
	EnemyType type;
	std::vector<DropInfo> drop_items;
};

// 3 main types of enemies

class WalkingEnemyInfo : public EnemyInfo {
public:
	WalkingEnemyInfo(EnemyType type, std::string name, int HP, int dmg, int defense, float jump_V0, float walk_V, glm::vec2 hitboxSize, glm::vec2 spriteSize) :
		EnemyInfo(type, name, HP, dmg, defense, hitboxSize, spriteSize), jump_V0{ jump_V0 }, walk_V{ walk_V } {
	}
	float get_jump_V0() const { return jump_V0; }
	float get_walk_V() const { return walk_V; }
private:
	float jump_V0, walk_V;
};

class FlyingEnemyInfo : public EnemyInfo {
public:
	FlyingEnemyInfo(EnemyType type, std::string name, int HP, int dmg, int defense, float flyingX_V, float flyingY_V, glm::vec2 hitboxSize, glm::vec2 spriteSize) :
		EnemyInfo(type, name, HP, dmg, defense, hitboxSize, spriteSize), flyingX_V{ flyingX_V }, flyingY_V{ flyingY_V } {
	}
	float get_moving_X_V() const override { return flyingX_V; }
	float get_moving_Y_V() const override { return flyingY_V; }
private:
	float flyingX_V, flyingY_V;
};

class WaterEnemyInfo : public EnemyInfo {

};