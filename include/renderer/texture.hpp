#ifndef __TEXTURE_H_
#define __TEXTURE_H_

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "gl.hpp"

#include <string>
#include <iostream>

class Texture {
    GLuint id;
    bool depth;
    unsigned int w, h;
    public:
        Texture(std::string filename) {
            SDL_Surface* tex_s = IMG_Load(filename.c_str());
            //SDL_SetColorKey(tex_s, 1, SDL_MapRGB(tex_s->format, 255, 0, 255));
            if(!tex_s) {
                const char *err = IMG_GetError();
                std::cout << err << std::endl;
                throw err;
            }
            glGenTextures(1, &id);
            glBindTexture(GL_TEXTURE_2D, id);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            GLenum channels = GL_RGB;
            if(tex_s->format->BytesPerPixel == 4) {
                channels = GL_RGBA;
            } else {
                channels = GL_RGB;
            }
            glTexImage2D(GL_TEXTURE_2D, 0, channels, tex_s->w, tex_s->h, 0, channels, GL_UNSIGNED_BYTE, tex_s->pixels);
            glGenerateMipmap(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, 0);

            depth = false;
            w = tex_s->w;
            h = tex_s->h;
            SDL_FreeSurface(tex_s);
        }
        Texture(unsigned char *bytes, unsigned int w, unsigned int h, bool alpha_channel = true) {
          //assumes pixel format is RGB or RGBA
            glGenTextures(1, &id);
            glBindTexture(GL_TEXTURE_2D, id);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            GLenum channels;
            if(alpha_channel) {
                channels = GL_RGBA;
            } else {
                channels = GL_RGB;
            }
            glTexImage2D(GL_TEXTURE_2D, 0, channels, w, h, 0, channels, GL_UNSIGNED_BYTE, bytes);
            glGenerateMipmap(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, 0);
            depth = false;
            this->w = w;
            this->h = h;
        }
        void bind(unsigned int texture_unit) {
            glActiveTexture(GL_TEXTURE0+texture_unit);
            glBindTexture(GL_TEXTURE_2D, id);
            if(!depth) {
                //glGenerateMipmap(GL_TEXTURE_2D);
            }
        }
        void unbind(unsigned int texture_unit) {
            glActiveTexture(GL_TEXTURE0+texture_unit);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
        unsigned int get_width() {
            return w;
        }
        unsigned int get_height() {
            return h;
        }

    //so that Framebuffer can generate internal textures
    friend class Framebuffer;
    private:
        Texture(unsigned int id, bool depth): id(id), depth(depth) {}
};

#endif // __TEXTURE_H_
