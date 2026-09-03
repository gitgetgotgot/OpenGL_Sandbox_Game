#include "Objects/ObjectFactory.h"
#include "Objects/ObjectManager.h"

CoreObject::ObjectInfo* CoreObject::_ObjectFactoryI::register_object_info(std::unique_ptr<CoreObject::ObjectInfo> new_object_info, const std::string& UID) {
	return CoreObject::ObjectManager::get_instance()->register_object_info(std::move(new_object_info), UID);
}

CoreObject::ObjectFactoryRegistry* CoreObject::ObjectFactoryRegistry::get_instance() {
	static CoreObject::ObjectFactoryRegistry registry;
	return &registry;
}

void CoreObject::ObjectFactoryRegistry::ClearData() {
	object_class_to_factory_id = {};
	std::vector<std::unique_ptr<_ObjectFactoryI>>().swap(factories);
}

void CoreObject::ObjectFactoryRegistry::register_factory(std::string class_uid, std::unique_ptr<_ObjectFactoryI> factory) {
	object_class_to_factory_id.emplace(class_uid, factories.size());
	factories.emplace_back(std::move(factory));
}

CoreObject::_ObjectFactoryI* CoreObject::ObjectFactoryRegistry::get_factory(uint32_t factory_id) {
	return factories[factory_id].get();
}

std::optional<uint32_t> CoreObject::ObjectFactoryRegistry::get_factory_id(std::string class_uid) {
	auto it = object_class_to_factory_id.find(class_uid);
	if (it == object_class_to_factory_id.end()) return std::nullopt;
	else return it->second;
}