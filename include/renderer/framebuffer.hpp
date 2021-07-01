#ifndef __FRAMEBUFFER_H_
#define __FRAMEBUFFER_H_

#include "gl.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include "resource_manager.hpp"

#include <string>
#include <iostream>

class Framebuffer {
    GLuint id;
    inline static GLuint vao = 0; //fullscreen quad
    Shader shader;

    Texture textures[2] = {{0, 0}, {0, 1}}; //color, depth
    const char* color_uniform;
    const char* depth_uniform;
    GLuint color_texture_unit;
    GLuint depth_texture_unit;

    unsigned int width, height;

    public:

        Framebuffer(unsigned int width, unsigned int height, std::string frag_filename,
                    const char* color_uniform = "framebuffer_color",
                    const char* depth_uniform = "framebuffer_depth",
                    unsigned int color_texture_unit = 0,
                    unsigned int depth_texture_unit = 1):
            shader(Shader(ResourceManager::canonicalize_shader("framebuffer.vert", frag_filename))),
            width(width), height(height),
            color_uniform(color_uniform), depth_uniform(depth_uniform), color_texture_unit(color_texture_unit), depth_texture_unit(depth_texture_unit) {

            glGenFramebuffers(1, &id);
            glBindFramebuffer(GL_FRAMEBUFFER, id);

            //set up textures
            GLuint tex_ids[2];
            glGenTextures(2, tex_ids);
            textures[0] = Texture(tex_ids[0], false);
            textures[1] = Texture(tex_ids[1], true);

            glBindTexture(GL_TEXTURE_2D, textures[0].id);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textures[0].id, 0);

            glBindTexture(GL_TEXTURE_2D, textures[1].id);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, textures[1].id, 0);

            //check for errors
            auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
            if(status != GL_FRAMEBUFFER_COMPLETE) {
                std::cout<<"Framebuffer not complete: "<<status<<std::endl;
            }
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            //generate vao if not already done.
            if(!vao) {
                glGenVertexArrays(1, &vao);
                glBindVertexArray(vao);
                std::vector<glm::vec3> vertices = {{-1.0f, -1.0f, 0.0f}, {1.0f, -1.0f, 0.0f}, {-1.0f, 1.0f, 0.0f},
                                                {1.0f, -1.0f, 0.0f}, {1.0f, 1.0f, 0.0f}, {-1.0f, 1.0f, 0.0f}};
                GLuint vbo;
                glGenBuffers(1, &vbo);
                glBindBuffer(GL_ARRAY_BUFFER, vbo);
                glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);
                glEnableVertexAttribArray(0);
                glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);
                glBindVertexArray(0);
            }
        }
        void bind() {
            glBindFramebuffer(GL_FRAMEBUFFER, id);
            glViewport(0, 0, width, height);
        }
        void clear(float r = 0.0, float g = 0.0, float b = 0.0, float a = 1.0) {
            glClearColor(r, g, b, a);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }
        void unbind() {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
        void draw() {
            glDisable(GL_DEPTH_TEST);
            glDisable(GL_CULL_FACE);
            shader.bind();
            if(color_uniform) {
                bind_color(color_texture_unit);
                shader.set_uniform(color_uniform, static_cast<signed int>(color_texture_unit));
            }
            if(depth_uniform) {
                bind_depth(depth_texture_unit);
                shader.set_uniform(depth_uniform, static_cast<signed int>(depth_texture_unit));
            }
            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            shader.unbind();
            glBindVertexArray(0);
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_CULL_FACE);
        }
        void color_attachment(const char* uniform_name, unsigned int texture_unit) {
            color_uniform = uniform_name;
            color_texture_unit = texture_unit;
        }
        void depth_attachment(const char* uniform_name, unsigned int texture_unit) {
            depth_uniform = uniform_name;
            depth_texture_unit = texture_unit;
        }
        void bind_color(unsigned int texture_unit) {
            glActiveTexture(GL_TEXTURE0 + texture_unit);
            glBindTexture(GL_TEXTURE_2D, textures[0].id);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        void bind_depth(unsigned int texture_unit) {
            glActiveTexture(GL_TEXTURE0 + texture_unit);
            glBindTexture(GL_TEXTURE_2D, textures[1].id);
        }
        Texture& color_texture() {
            return textures[0];
        }
        Texture& depth_texture() {
            return textures[1];
        }
        void attach_texture(std::string name, Texture& texture) {
            shader.bind();
            shader.attach_texture(name, texture);
        }
        template<typename T>
        void set_uniform(std::string name, T value) {
            shader.bind();
            shader.set_uniform(name, value);
        }
        unsigned int get_width() {
            return width;
        }
        unsigned int get_height() {
            return height;
        }
};

#endif // __FRAMEBUFFER_H_
