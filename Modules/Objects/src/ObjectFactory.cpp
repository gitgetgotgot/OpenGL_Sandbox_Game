#include "Objects/ObjectFactory.h"
#include "Objects/ObjectManager.h"

Engine::Object::ObjectInfo* Engine::Object::_ObjectFactoryI::register_object_info(std::unique_ptr<Engine::Object::ObjectInfo> new_object_info, const std::string& UID) {
	return Engine::Object::ObjectManager::Instance().register_object_info(std::move(new_object_info), UID);
}

void Engine::Object::ObjectFactoryRegistry::ClearData() {
	object_class_to_factory_id = {};
	std::vector<std::unique_ptr<_ObjectFactoryI>>().swap(factories);
}

void Engine::Object::ObjectFactoryRegistry::register_factory(std::string class_uid, std::unique_ptr<_ObjectFactoryI> factory) {
	object_class_to_factory_id.emplace(class_uid, factories.size());
	factories.emplace_back(std::move(factory));
}

Engine::Object::_ObjectFactoryI* Engine::Object::ObjectFactoryRegistry::get_factory(uint32_t factory_id) {
	return factories[factory_id].get();
}

std::optional<uint32_t> Engine::Object::ObjectFactoryRegistry::get_factory_id(std::string class_uid) {
	auto it = object_class_to_factory_id.find(class_uid);
	if (it == object_class_to_factory_id.end()) return std::nullopt;
	else return it->second;
}