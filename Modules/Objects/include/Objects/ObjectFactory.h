#pragma once
#include "Objects/ObjectInfo.h"

namespace CoreObject {
	class _ObjectFactoryI {
	public:
		virtual void add_object(const DataNode& data) = 0;
		virtual ~_ObjectFactoryI() {}
	protected:
		ObjectInfo* register_object_info(std::unique_ptr<ObjectInfo> new_object_info, const std::string& UID);
	};

	template<typename T, ObjectType MAIN_TYPE = ObjectType::None>
	class ObjectInfoFactory : public _ObjectFactoryI {
	public:
		static_assert(std::is_base_of_v<ObjectInfo, T>, "T must inherit from ObjectInfo");

		void add_object(const DataNode& data) override {
			T* info = create_object_info(data);
			info->fill_data(data);
		}
	protected:
		T* create_object_info(const DataNode& data) {
			std::string UID = data["UID"].get_as<std::string>();
			std::unique_ptr<ObjectInfo> new_info = std::make_unique<T>(MAIN_TYPE);
			return static_cast<T*>(register_object_info(std::move(new_info), UID));
		}
	};

	class ObjectFactoryRegistry {
	public:
		static ObjectFactoryRegistry& get_instance() {
			static ObjectFactoryRegistry registry;
			return registry;
		}
		void ClearData();
		void register_factory(std::string class_uid, std::unique_ptr<_ObjectFactoryI> factory);
		_ObjectFactoryI* get_factory(uint32_t factory_id);
		std::optional<uint32_t> get_factory_id(std::string class_uid);
	private:
		std::unordered_map<std::string, uint32_t> object_class_to_factory_id;
		std::vector<std::unique_ptr<_ObjectFactoryI>> factories;
		ObjectFactoryRegistry() {}
		~ObjectFactoryRegistry() {}
	};
}