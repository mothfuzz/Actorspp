#ifndef __APP_H_
#define __APP_H_

#include <SDL_image.h>
#include <string>

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "framebuffer.hpp"

class Window {
    static inline Window* self = nullptr;
    SDL_Window* window;
    SDL_GLContext gl_context;
    Framebuffer* fb;
    bool fullscreen = false;
    int ticks = 0;
    int frames = 0;
    public:
        Window() {
            if(self) {
                throw std::runtime_error("Window already exists!");
            }
            self = this;
            //std::string path = ".";
            //for (const auto & entry : std::filesystem::recursive_directory_iterator(path))
                //std::cout << entry.path() << std::endl;

            std::cout<<"initializing SDL..."<<std::endl;
            SDL_Init(SDL_INIT_EVERYTHING);
            IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG | IMG_INIT_TIF);

            std::cout<<"setting SDL GL attributes..."<<std::endl;
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

            SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
            SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
            SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
            //SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
            //SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

            std::cout<<"creating a new window..."<<std::endl;
            window = SDL_CreateWindow("Hello!!",
                                    SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                    640, 400,
                                    SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
            std::cout<<"setting up GL context..."<<std::endl;
            gl_context = SDL_GL_CreateContext(window);
            SDL_GL_MakeCurrent(window, gl_context);
            SDL_GL_SetSwapInterval(1);

            gladLoadGL();

            std::cout<<"configuring OpenGL..."<<std::endl;
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_CULL_FACE);

            fb = new Framebuffer(640, 400, "framebuffer.frag");
        }
        ~Window() {
            SDL_GL_DeleteContext(gl_context);
            SDL_DestroyWindow(window);
            SDL_Quit();
        }
        bool poll_events() {
            SDL_Event e;
            while(SDL_PollEvent(&e)) {
                if(e.type == SDL_QUIT) {
                    return false;
                }
                if(e.type == SDL_KEYDOWN) {
                    if(e.key.keysym.sym == SDLK_F4) {
                        if(fullscreen) {
                            SDL_SetWindowFullscreen(window, 0);
                        } else {
                            SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
                        }
                        fullscreen = !fullscreen;
                    }
                }
            }
            return true;
        }
        void set_resolution(int width, int height) {
            delete fb;
            fb = new Framebuffer(width, height, "framebuffer.frag");
        }
        static int get_width() {
            return Window::self->fb->get_width();
        }
        static int get_height() {
            return Window::self->fb->get_height();
        }
        void set_window_size(int width, int height) {
            SDL_SetWindowSize(window, width, height);
        }
        static int get_window_width() {
            int w;
            SDL_GetWindowSize(Window::self->window, &w, 0);
            return w;
        }
        static int get_window_height() {
            int h;
            SDL_GetWindowSize(Window::self->window, 0, &h);
            return h;
        }
        static bool is_key_down(std::string name) {
            const Uint8 *state = SDL_GetKeyboardState(NULL);
            return static_cast<bool>(state[SDL_GetScancodeFromName(name.c_str())]);
        }
        static std::function<bool(void)> track_keypress(std::string key) {
            bool clicked = false;
            return [clicked, key]() mutable -> bool {
                if (Window::is_key_down(key)) {
                    if (!clicked) {
                        clicked = true;
                        return true;
                    }
                    else {
                        return false;
                    }
                }
                else {
                    clicked = false;
                    return false;
                }
            };
        }
        static std::function<bool(void)> track_click(int button) {
            bool clicked = false;
            return [clicked, button]() mutable -> bool {
                if (Window::is_mousebutton_down(button)) {
                    if (!clicked) {
                        clicked = true;
                        return true;
                    }
                    else {
                        return false;
                    }
                }
                else {
                    clicked = false;
                    return false;
                }
            };
        }
        static bool is_mousebutton_down(int button) {
            const Uint8 state = SDL_GetMouseState(NULL, NULL);
            return static_cast<bool>(state & SDL_BUTTON(button));
        }
        static glm::vec2 raw_mouse_position() {
            int x, y;
            SDL_GetMouseState(&x, &y);
            return { x, y };
        }
        static glm::vec2 mouse_position() {
            glm::vec2 raw_pos = raw_mouse_position();
            raw_pos.x /= get_window_width();
            raw_pos.y /= get_window_height();
            raw_pos.x *= get_width();
            raw_pos.y *= -get_height();
            raw_pos.x -= get_width() / 2.0f;
            raw_pos.y += get_height() / 2.0f;
            return raw_pos;
        }

        void clear() {
            fb->bind();

            //render new frame
            fb->clear(100.0/255.0, 149.0/255.0, 237.0/255.0, 1.0);
        }

        void draw() {
            fb->unbind();
            int w, h;
            SDL_GetWindowSize(window, &w, &h);

            float frame_width = fb->get_width();
            float frame_height = fb->get_height();
            float window_width = w;
            float window_height = h;

            float frame_ar = frame_width / frame_height;
            float window_ar = window_width / window_height;
            //std::cout << "frame's ar is " << frame_ar << std::endl;
            //std::cout << "windows' ar is " << window_ar << std::endl;

            if(frame_ar == window_ar) {
                glViewport(0, 0, w, h);
            }
            //window is wider than framebuffer, so have to letterbox
            if(window_ar > frame_ar) {
                int letterbox = window_width - window_height*frame_ar;
                //std::cout << "letterboxing by " << letterbox << " pixels" << std::endl;
                glViewport(letterbox/2, 0, w-letterbox, h);
            }
            //window is taller than framebuffer, so pillarbox
            if(frame_ar > window_ar) {
                int pillarbox = window_height - window_width/frame_ar;
                //std::cout << "pillarboxing by " << pillarbox << " pixels" << std::endl;
                glViewport(0, pillarbox/2, w, h-pillarbox);
            }

            glClearColor(0.0, 0.0, 0.0, 1.0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            fb->draw();

            // swap
            SDL_GL_SwapWindow(window);

            while (GLenum err = glGetError()) {
                const char *errs = "";
                switch(err) {
                    case GL_NO_ERROR:
                        break;
                    case GL_INVALID_ENUM:
                        errs = "GL_INVALID_ENUM";
                        break;
                    case GL_INVALID_VALUE:
                        errs = "GL_INVALID_VALUE";
                        break;
                    case GL_INVALID_OPERATION:
                        errs = "GL_INVALID_OPERATION";
                        break;
                    case GL_INVALID_FRAMEBUFFER_OPERATION:
                        errs = "GL_INVALID_FRAMEBUFFER_OPERATION";
                        break;
                    case GL_OUT_OF_MEMORY:
                        errs = "GL_OUT_OF_MEMORY";
                        break;
                }
                std::cout<<"GL Error: "<<errs<<std::endl;
            }

            int now = SDL_GetTicks();
            if(now > ticks + 1000) {
                std::stringstream title;
                title << "FPS: " << frames;
                SDL_SetWindowTitle(window, title.str().c_str());
                frames = 0;
                ticks = now;
            }
            frames++;
        }
};

#endif // __APP_H_
