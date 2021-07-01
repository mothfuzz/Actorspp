#ifndef __COMPONENTS_H_
#define __COMPONENTS_H_

#include <unordered_map>
#include <vector>

template<typename T>
class Component {
    inline static std::vector<T> data = {};
    inline static std::unordered_map<int, typename std::vector<T>::size_type> actor_entries = {};
    inline static std::unordered_map<int, int> generations = {};
    inline static std::vector<int> id_keys = {};

    int id;
    int generation;

    static void remove(int id) {
        if (actor_entries[id] != data.size() - 1) {
            int end_id = std::find_if(std::begin(actor_entries), std::end(actor_entries), [](auto& p) {
                return p.second == data.size() - 1;
            })->first;
            std::iter_swap(data.begin() + actor_entries[id], data.end() - 1);
            std::iter_swap(id_keys.begin() + actor_entries[id], id_keys.end() - 1);
            actor_entries[end_id] = actor_entries[id];
        }
        data.pop_back();
        id_keys.pop_back();
        actor_entries.erase(id);
        generations[id]++;
    }

    public:
        Component() = delete;
        Component(const Component& e) = delete;
        Component& operator=(const Component& e) = delete;
        Component& operator=(Component&& e) {
            id = e.id;
            e.id = 0;
        }
        Component(Component&& e): id(e.id) {
            e.id = 0;
        }
        Component(int id, T entry): id(id) {
            if (contains(id)) {
                remove(id);
            }
            actor_entries.insert({ id, data.size() });
            data.push_back(entry);
            id_keys.push_back(id);
            generation = generations[id];
        }
        Component(int id): Component(id, {}) {}
        ~Component() {
            if (id && generation == generations[id]) {
                remove(id);
            }
        }
        static bool contains(int id) {
            return actor_entries.find(id) != actor_entries.end();
        }
        T* operator()() {
            return entry(id);
        }
        T* operator->() {
            return this->operator()();
        }

        static std::unordered_map<int, T*> entries() {
            std::unordered_map<int, T*> m = {};
            for(auto& [id, i]: actor_entries) {
                m[id] = &data[i];
            }
            return m;
        }
        //can be faster than using entries() though shouldn't make too too much of a difference
        static std::vector<int>& keys() {
            return id_keys;
        }
        static T* entry(int id) {
            if(contains(id)) {
                return &data[actor_entries[id]];
            } else {
                return nullptr;
            }
        }
};

#endif // __COMPONENTS_H_
