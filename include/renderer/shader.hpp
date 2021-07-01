#ifndef __SHADER_H_
#define __SHADER_H_

#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>

#include <glm/glm.hpp>

#include "gl.hpp"

#include "texture.hpp"

class Renderer;

class Shader {
    friend class Renderer;
    GLuint id;
    unsigned int next_tu = 0; //next free texture unit
    std::unordered_map<std::string, unsigned int> assigned_textures; //currently assigned texture units
    std::unordered_map<std::string, int> uniform_locations;
    public:
        Shader(std::string vert_filename, std::string frag_filename) {

            int success;
            char error_log[512];

            std::ifstream vert_file(vert_filename);
            if(!vert_file.is_open()) {
                throw std::runtime_error("unable to open "+vert_filename);
            }
            std::stringstream vert_ss;
            vert_ss << vert_file.rdbuf();
            std::string vert_src = vert_ss.str();
            //std::cout<<"vertex shader source:"<<std::endl;
            //std::cout<<vert_src<<std::endl;
            const char* vert_src_s = vert_src.c_str();
            GLuint vert = glCreateShader(GL_VERTEX_SHADER);
            glShaderSource(vert, 1, &vert_src_s, NULL);
            glCompileShader(vert);

            glGetShaderiv(vert, GL_COMPILE_STATUS, &success);
            if(!success) {
                glGetShaderInfoLog(vert, 512, NULL, error_log);
                std::cout<<"Error in vertex shader: "<<error_log<<std::endl;
            }

            std::ifstream frag_file(frag_filename);
            if(!frag_file.is_open()) {
                throw std::runtime_error("unable to open "+frag_filename);
            }
            std::stringstream frag_ss;
            frag_ss << frag_file.rdbuf();
            std::string frag_src = frag_ss.str();
            //std::cout<<"fragment shader source:"<<std::endl;
            //std::cout<<frag_src<<std::endl;
            const char* frag_src_s = frag_src.c_str();
            GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(frag, 1, &frag_src_s, NULL);
            glCompileShader(frag);

            glGetShaderiv(frag, GL_COMPILE_STATUS, &success);
            if(!success) {
                glGetShaderInfoLog(frag, 512, NULL, error_log);
                std::cout<<"Error in fragment shader: "<<error_log<<std::endl;
            }

            id = glCreateProgram();
            glBindAttribLocation(id, 0, "vertex");
            glBindAttribLocation(id, 1, "texcoord");
            glBindAttribLocation(id, 2, "normal");
            glBindAttribLocation(id, 3, "tangent");
            glAttachShader(id, vert);
            glAttachShader(id, frag);
            glLinkProgram(id);

            glGetProgramiv(id, GL_LINK_STATUS, &success);
            if(!success) {
                glGetProgramInfoLog(id, 512, NULL, error_log);
                std::cout<<"Error in shader program: "<<error_log<<std::endl;
            }

            glDeleteShader(vert);
            glDeleteShader(frag);
        }
        Shader(std::pair<std::string, std::string> filename): Shader(filename.first, filename.second) {};

        template<typename T>
        void set_uniform(std::string name, T value);

        template<>
        void set_uniform(std::string name, glm::vec2 value) {
            if (uniform_locations.find(name) == uniform_locations.end()) {
                uniform_locations[name] = glGetUniformLocation(id, name.c_str());
            }
            glUniform2fv(uniform_locations[name], 1, &value[0]);
        }
        template<>
        void set_uniform(std::string name, glm::vec3 value) {
            if (uniform_locations.find(name) == uniform_locations.end()) {
                uniform_locations[name] = glGetUniformLocation(id, name.c_str());
            }
            glUniform3fv(uniform_locations[name], 1, &value[0]);
        }
        template<>
        void set_uniform(std::string name, glm::vec4 value) {
            if (uniform_locations.find(name) == uniform_locations.end()) {
                uniform_locations[name] = glGetUniformLocation(id, name.c_str());
            }
            glUniform4fv(uniform_locations[name], 1, &value[0]);
        }
        template<>
        void set_uniform(std::string name, glm::mat2 value) {
            if (uniform_locations.find(name) == uniform_locations.end()) {
                uniform_locations[name] = glGetUniformLocation(id, name.c_str());
            }
            glUniformMatrix2fv(uniform_locations[name], 1, GL_FALSE, &value[0][0]);
        }
        template<>
        void set_uniform(std::string name, glm::mat3 value) {
            if (uniform_locations.find(name) == uniform_locations.end()) {
                uniform_locations[name] = glGetUniformLocation(id, name.c_str());
            }
            glUniformMatrix3fv(uniform_locations[name], 1, GL_FALSE, &value[0][0]);
        }
        template<>
        void set_uniform(std::string name, glm::mat4 value) {
            if (uniform_locations.find(name) == uniform_locations.end()) {
                uniform_locations[name] = glGetUniformLocation(id, name.c_str());
            }
            glUniformMatrix4fv(uniform_locations[name], 1, GL_FALSE, &value[0][0]);
        }
        template<>
        void set_uniform(std::string name, std::array<float, 2> value) {
            if (uniform_locations.find(name) == uniform_locations.end()) {
                uniform_locations[name] = glGetUniformLocation(id, name.c_str());
            }
            glUniform2fv(uniform_locations[name], 1, &value[0]);
        }
        template<>
        void set_uniform(std::string name, std::array<float, 3> value) {
            if (uniform_locations.find(name) == uniform_locations.end()) {
                uniform_locations[name] = glGetUniformLocation(id, name.c_str());
            }
            glUniform3fv(uniform_locations[name], 1, &value[0]);
        }
        template<>
        void set_uniform(std::string name, std::array<float, 4> value) {
            if (uniform_locations.find(name) == uniform_locations.end()) {
                uniform_locations[name] = glGetUniformLocation(id, name.c_str());
            }
            glUniform4fv(uniform_locations[name], 1, &value[0]);
        }
        template<>
        void set_uniform(std::string name, std::array<std::array<float, 2>, 2> value) {
            if (uniform_locations.find(name) == uniform_locations.end()) {
                uniform_locations[name] = glGetUniformLocation(id, name.c_str());
            }
            glUniformMatrix2fv(uniform_locations[name], 1, GL_FALSE, &value[0][0]);
        }
        template<>
        void set_uniform(std::string name, std::array<std::array<float, 3>, 3> value) {
            if (uniform_locations.find(name) == uniform_locations.end()) {
                uniform_locations[name] = glGetUniformLocation(id, name.c_str());
            }
            glUniformMatrix3fv(uniform_locations[name], 1, GL_FALSE, &value[0][0]);
        }
        template<>
        void set_uniform(std::string name, std::array<std::array<float, 4>, 4> value) {
            if (uniform_locations.find(name) == uniform_locations.end()) {
                uniform_locations[name] = glGetUniformLocation(id, name.c_str());
            }
            glUniformMatrix4fv(uniform_locations[name], 1, GL_FALSE, &value[0][0]);
        }
        template<>
        void set_uniform(std::string name, int value) {
            if (uniform_locations.find(name) == uniform_locations.end()) {
                uniform_locations[name] = glGetUniformLocation(id, name.c_str());
            }
            glUniform1i(uniform_locations[name], value);
        }
        template<>
        void set_uniform(std::string name, float value) {
            if (uniform_locations.find(name) == uniform_locations.end()) {
                uniform_locations[name] = glGetUniformLocation(id, name.c_str());
            }
            glUniform1f(uniform_locations[name], value);
        }
        template<>
        void set_uniform(std::string name, bool value) {
            if (uniform_locations.find(name) == uniform_locations.end()) {
                uniform_locations[name] = glGetUniformLocation(id, name.c_str());
            }
            glUniform1i(uniform_locations[name], value);
        }

        void attach_texture(std::string name, Texture& texture) {
            if(assigned_textures.find(name) == assigned_textures.end()) {
                texture.bind(next_tu);
                assigned_textures[name] = next_tu++;
            } else {
                texture.bind(assigned_textures[name]);
            }
            set_uniform(name, static_cast<signed int>(assigned_textures[name]));
        }


        void bind() {
            GLint id_in_use;
            glGetIntegerv(GL_CURRENT_PROGRAM, &id_in_use);
            if (id != id_in_use) {
                glUseProgram(id);
            }
        }

        void unbind() {
            glUseProgram(0);
            next_tu = 0;
            assigned_textures = {};
        }
};

#endif // __SHADER_H_
