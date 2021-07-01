#ifndef __ACTORS_H_
#define __ACTORS_H_

#include <unordered_map>
#include <iostream>

#include "components.hpp"

class Actor {
    inline static int base_id = 1;
    inline static std::unordered_map<int, Actor*> actors = {};

    public:
        const int id = base_id++;
        Actor() {
            actors[id] = this;
        }
        ~Actor() {
            actors.erase(id);
        }
        virtual void update() {}

        template<typename T>
        static const T* find(int id) {
            auto iter = actors.find(id);
            if(iter == actors.end()) {
                return nullptr;
            } else {
                return dynamic_cast<T*>(iter->second);
            }
        }

        template<typename T>
        bool has() {
            return Component<T>::contains(id);
        }

        static void update_all() {
            for(auto& [id, actor]: actors) {
                actor->update();
            }
        }
};

#endif // __ACTORS_H_
