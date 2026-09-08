#pragma once
#include <Utility/ResourceData.h>
#include <Utility/DynamicArray.h>
#include <Utility/DataParser.h>

namespace CoreEntity {
	enum EntityMainType : uint8_t {
		isMob, isProjectile, isPet, isNPC
	};

	class EntityInfo {
	public:
		EntityInfo(EntityMainType type);
		virtual ~EntityInfo() {}
		void _set_UID(std::string_view uid);
		bool _has_tag(const uint16_t& TAG) const;
		void _sort_tags() const;
		virtual void fill_data(const DataNode& data);
		virtual void fill_dependent_data(const DataNode& data);
	public:
		EntityMainType main_type = EntityMainType::isMob;
		std::string_view uid, name;
		uint16_t factory_ID = 0;
		DynamicArray<uint16_t> tags;
		DynamicArray<uint16_t> lights;
	};

	/*
	struct DroppedItem {
		bool has_pick_cd = false; //if player is throwing something, then it has cd for 1 second for picking it up
		float cd_time = 0.f;
		int id = 0;
		int amount = 0;
		Collider_2D_AABB hitbox;
		//physics
		bool has_bottom_collision = false;
		float fallingDistance = 0.f;
		float time_in_free_falling = 0.f;
		float Xinc = 0.f;
	};
	*/
}