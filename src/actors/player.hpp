#ifndef __PLAYER_H_
#define __PLAYER_H_

#include <iostream>

#include "actors.hpp"
#include "events.hpp"

struct Damage {
    int value;
};

class Player: public Actor {
    EventQueue<Damage> damages;
    public:
        int health;
        Player(float health=100): health(health), damages(id) {
            std::cout << "new player: " << this->id << std::endl;
        }
        void update() override {
            while(auto event = damages.next()) {
                std::cout << "ouch!! -" << event->value << "hp" << std::endl;
                health -= event->value;
            }
        }
};

#endif // __PLAYER_H_
