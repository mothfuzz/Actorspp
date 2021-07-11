#include <iostream>

#include "actors.hpp"
#include "events.hpp"
#include "components.hpp"
#include "bindings.hpp"

#include "renderer.hpp"

#include "player.hpp"

#include "stress_tests.hpp"

class Bulba : public Actor {
    Component<Model> m = { id, {"bulbasaur.obj"} };
    Component<Material> mat = { id, {DEFAULT_SHADER, {
        {"albedo_tex", "bulbasaur.png"},
        {"normal_tex", "default_normal.png"},
        {"glossy_tex", "default_glossy.png"},
    }}};
    Component<Transform> t = { id, TRANSFORM_ORIGIN };
    void update() override {
        t->rotation.y += 0.5;
    }
public:
    Bulba() {
        t->scale = { 2.0, 2.0, 2.0 };
        t->position.y = -64.0;
    }
};

class Player3D: public Actor {

        Component<Model> m = {id, {"wall.obj"}};
        Component<Material> mat = {id, {DEFAULT_SHADER, {
            {"albedo_tex", "stonewall/albedo.jpg"},
            {"normal_tex", "stonewall/normal.jpg"},
            {"glossy_tex", "stonewall/roughness.jpg"}
        }}};
        Component<Transform> t = {id, TRANSFORM_ORIGIN};
        Component<Camera> cam = { id };

        void update() override {
            float speed = 4.0f;
            float c = cos(t->rotation.y);
            float s = sin(t->rotation.y);
            if (Window::is_key_down("w")) {
                t->position.z -= speed * c;
                t->position.x += speed * s;
            }
            if (Window::is_key_down("s")) {
                t->position.z += speed * c;
                t->position.x -= speed * s;
            }
            if(Window::is_key_down("a")) {
                t->position.x -= speed * c;
                t->position.z -= speed * s;
            }
            if(Window::is_key_down("d")) {
                t->position.x += speed * c;
                t->position.z += speed * s;
            }
            if (Window::is_key_down("left")) {
                t->rotation.y -= 0.05;
            }
            if (Window::is_key_down("right")) {
                t->rotation.y += 0.05;
            }
        }

    public:
        Player3D() {
            t->scale = {128.0f, 128.0f, 128.0f};
            cam->set_active();
        }
};


class DamageQueue: public EventQueue<Damage> {};


void normal_main_loop() {

    ResourceManager resource_manager{ "../resources/" };
    Window win;
    Renderer ren;
    win.set_resolution(320, 240);

    PyVM vm;

    Bulba b;
    //Player3D p;
    //ren.set_active_camera(p);

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
