#pragma once
#include <vector>

template<typename T>
class sparse_set {
public:
	sparse_set() {}
	T* add(T&& comp, uint32_t id) {
		if (id >= id_set.size()) {
			id_set.resize(id + 1, 0);
		}
		id_set[id] = dense_ids.size() + 1;
		dense_ids.emplace_back(id);
		dense_components.emplace_back(std::move(comp));
		return &dense_components.back();
	}
	T* get(uint32_t id) {
		if (id < id_set.size()) {
			uint32_t index = id_set[id];
			if (index > 0) {
				return &dense_components[index - 1];
			}
		}
		return nullptr;
	}
	bool remove(uint32_t id) {
		if (id >= id_set.size()) return false; //id is bigger than size
		int index = id_set[id] - 1;
		if (index == -1) return false; //no such component exists in dense comps
		int last_index = dense_ids.size() - 1;

		std::swap(dense_ids[index], dense_ids[last_index]);
		std::swap(dense_components[index], dense_components[last_index]);

		id_set[dense_ids[index]] = index + 1;
		dense_ids.pop_back();
		dense_components.pop_back();
		id_set[id] = 0;
		return true;
	}
	void reserve(uint32_t count) {
		id_set.reserve(count);
		dense_ids.reserve(count);
		dense_components.reserve(count);
	}
	void clear() {
		id_set.clear();
		dense_ids.clear();
		dense_components.clear();
	}
	T& operator[](int i) {
		return dense_components[i];
	}
	size_t size() {
		return dense_components.size();
	}
	T* data() {
		return dense_components.data();
	}
private:
	std::vector<uint32_t> id_set;
	std::vector<uint32_t> dense_ids;
	std::vector<T> dense_components;
};
