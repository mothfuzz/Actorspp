#include <random>
#include <functional>

#include "actors.hpp"
#include "components.hpp"

#include "renderer.hpp"

static std::default_random_engine rng;

static int X_MIN = -1920 / 2;
static int X_MAX = 1920 / 2;
static int Y_MIN = -1080 / 2;
static int Y_MAX = 1080 / 2;

class StressTest : public Actor {
    //Component<Sprite> spr = {id, {"player.png", 16, 16}};
    Component<Model> m = { id, {"cube.obj"} };
    Component<Material> mat = { id, {DEFAULT_SHADER, {
        {ALBEDO_TEX, "stonewall/albedo.jpg"},
        {NORMAL_TEX, "stonewall/normal.jpg"},
        {GLOSSY_TEX, "stonewall/roughness.jpg"}
    }} };
    Component<Transform> t = { id, TRANSFORM_ORIGIN };
    float speed = 4.0f;
    inline static std::function<int(void)> rand_wall = std::bind(std::uniform_int_distribution<int>{1, 4}, rng);
    inline static std::function<int(void)> rand_x = std::bind(std::uniform_int_distribution<int>{X_MIN, X_MAX}, rng);
    inline static std::function<int(void)> rand_y = std::bind(std::uniform_int_distribution<int>{Y_MIN, Y_MAX}, rng);
    inline static std::function<int(void)> rand_dir = std::bind(std::uniform_int_distribution<int>{0, 360}, rng);
    void reinit() {
        auto& pos = t->position;
        auto& rot = t->rotation.z;
        int wall = rand_wall();
        switch (wall) {
        case 1:
            pos.x = X_MIN - 8;
            pos.y = rand_y();
            break;
        case 2:
            pos.x = X_MAX + 8;
            pos.y = rand_y();
            break;
        case 3:
            pos.x = rand_x();
            pos.y = Y_MIN - 8;
            break;
        case 4:
            pos.x = rand_x();
            pos.y = Y_MAX + 8;
            break;
        }
        rot = rand_dir();
    }
public:
    StressTest() {
        t->scale = { 32.0f, 32.0f, 32.0f };
        reinit();
    }
    void update() override {
        auto& pos = t->position;
        auto& rot = t->rotation.z;
        if (pos.x > X_MAX + 8 || pos.x < X_MIN - 8 || pos.y > Y_MAX + 8 || pos.y < Y_MIN - 8) {
            reinit();
        }
        pos.x += cos(glm::radians(rot)) * speed;
        pos.y += sin(glm::radians(rot)) * speed;
    }
};

#define NUM_BOYS 10000

inline void stress_test_sdl2() {
    SDL_Init(SDL_INIT_EVERYTHING);
    IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG | IMG_INIT_TIF);
    SDL_Window* win = nullptr;
    SDL_Renderer* ren = nullptr;

    X_MIN = 0;
    Y_MIN = 0;
    X_MAX = 640;
    Y_MAX = 400;

    win = SDL_CreateWindow("hewwo!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, X_MAX, Y_MAX, SDL_WINDOW_SHOWN);
    ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    SDL_RenderSetLogicalSize(ren, X_MAX, Y_MAX);

    SDL_Surface* s = IMG_Load("../resources/textures/player.png");
    SDL_SetColorKey(s, 1, SDL_MapRGB(s->format, 255, 0, 255));
    SDL_Texture* tex = SDL_CreateTextureFromSurface(ren, s);
    SDL_FreeSurface(s);

    std::vector<StressTest*> sts;
    for (int i = 0; i < NUM_BOYS; ++i) {
        sts.push_back(new StressTest{});
    }

    int frames = 0;
    int ticks = 0;
    SDL_Event e;
    bool running = true;
    while (running) {
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
            case SDL_QUIT:
                running = false;
                break;
            case SDL_KEYDOWN:
                if (e.key.keysym.sym == SDLK_ESCAPE) {
                    running = false;
                }
                break;
            }
        }

        Actor::update_all();

        SDL_SetRenderDrawColor(ren, 255, 0, 255, 255);
        SDL_RenderClear(ren);

        for (int id : Component<Sprite>::keys()) {
            Sprite* s = Component<Sprite>::entry(id);
            Transform* t = Component<Transform>::entry(id);
            SDL_Rect dst = { (int)t->position.x, (int)t->position.y, (int)s->width, (int)s->height };
            SDL_RenderCopyEx(ren, tex, NULL, &dst, t->rotation.z, NULL, SDL_FLIP_NONE);
        }

        SDL_RenderPresent(ren);

        int now = SDL_GetTicks();
        if (now > ticks + 1000) {
            std::stringstream title;
            title << "FPS: " << frames;
            SDL_SetWindowTitle(win, title.str().c_str());
            frames = 0;
            ticks = now;
        }
        frames++;
    }

    for (auto s : sts) {
        delete s;
    }
}

inline void stress_test_gl() {
    ResourceManager resource_manager{ "../resources/" };
    Window win;
    win.set_resolution(1920, 1080);
    Renderer ren;

    std::vector<StressTest*> sts;
    for (int i = 0; i < NUM_BOYS; ++i) {
        sts.push_back(new StressTest{});
    }

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

    for (auto s : sts) {
        delete s;
    }
}
