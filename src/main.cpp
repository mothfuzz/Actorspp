#include <iostream>

#include "actors.hpp"
#include "events.hpp"
#include "components.hpp"
#include "bindings.hpp"

#include "renderer.hpp"

#include "player.hpp"


//TODO: drag in ur renderer from uhhhh newstuff I think?? the nice 2d/3d one with shaders and stuff.

/*
** actual entities:
** model
** sprite
** transform
** rigidbody
** material (non-default)
** camera
** light
** resources:
** shader
** texture
** mesh
** non-entity-objects:
** framebuffer
** services:
** renderer -
** set active framebuffer
** set active camera
** draw model set
** draw sprite set
**
*/

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
    vm.load_file("../scripts/test.py");

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

    return 0;
}
