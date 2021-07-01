#ifndef __EVENTS_H_
#define __EVENTS_H_

#include <unordered_map>
#include <queue>
#include <optional>

template<typename T>
class EventQueue {
    inline static std::unordered_map<int, std::queue<T>> events = {};
    int id;
    public:
        EventQueue(int id): id(id) {
            events[id] = {};
        }
        ~EventQueue() {
            events.erase(id);
        }
        std::optional<T> next() {
            if(events[id].empty()) {
                return std::nullopt;
            } else {
                auto event = events[id].front();
                events[id].pop();
                return event;
            }
        }
        static void dispatch(int id, T event) {
            events[id].push(event);
        }
        static void broadcast(T event) {
            for(auto& [id, queue]: events) {
                queue.push(event);
            }
        }
};

template<typename T>
void dispatch(int id, T event) {
    EventQueue<T>::dispatch(id, event);
}
template<typename T>
void broadcast(T event) {
    EventQueue<T>::broadcast(event);
}

#endif // __EVENTS_H_
