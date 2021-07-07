#include <iostream>

#include "actors.hpp"
#include "events.hpp"
#include "components.hpp"
#include "bindings.hpp"

#include "renderer.hpp"

#include "player.hpp"

#include "stress_tests.hpp"

class Player3D: public Actor {

        Component<Model> m = {id, {"cube.obj"}};
        Component<Material> mat = {id, {DEFAULT_SHADER, {
            {"albedo_tex", "stonewall/albedo.jpg"},
            {"normal_tex", "stonewall/normal.jpg"},
            {"glossy_tex", "stonewall/roughness.jpg"}
        }}};
        Component<Transform> t = {id, TRANSFORM_ORIGIN};

        void update() override {
            float speed = 4.0f;
            if (Window::is_key_down("s")) {
                t->position.z -= speed;
            }
            if (Window::is_key_down("w")) {
                t->position.y += speed;
            }
            if(Window::is_key_down("a")) {
                t->position.x-=speed;
            }
            if(Window::is_key_down("d")) {
                t->position.x+=speed;
            }
        }

    public:
        Player3D() {
            t->scale = {128.0f, 128.0f, 128.0f};
        }
};


class DamageQueue: public EventQueue<Damage> {};


void normal_main_loop() {

    ResourceManager resource_manager{ "../resources/" };
    Window win;
    Renderer ren;

    PyVM vm;

    while (win.poll_events()) {
        if (Window::is_key_down("escape")) {
            break;
        }
        Actor::update_all();
        win.clear();
        //do some stuff
        ren.render_all();
        win.draw();
    }
}

int main(int argc, char *argv[]) {

    normal_main_loop();
    //stress_test_gl();
    //stress_test_sdl2();

    return 0;
}
