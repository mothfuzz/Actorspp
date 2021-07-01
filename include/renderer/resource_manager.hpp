#ifndef __RESOURCE_MANAGER_H_
#define __RESOURCE_MANAGER_H_

#include <unordered_map>
#include <stdexcept>
#include <iostream>

#include "texture.hpp"
#include "shader.hpp"
#include "mesh.hpp"

class ResourceManager {
    inline static ResourceManager *self = nullptr;
    std::string base_path;
    //in order of cost-per-bind
    std::unordered_map<std::string, Shader*> shaders;
    std::unordered_map<std::string, Texture*> textures;
    std::unordered_map<std::string, Mesh*> meshes;
public:
        ResourceManager(std::string base_path): base_path(base_path) {
            if(self == nullptr) {
                self = this;
            } else {
                throw std::runtime_error("Resource Manager already exists!");
            }
        }
        ~ResourceManager() {
            for(auto& [key, shader]: shaders) {
                delete shader;
            }
            for(auto& [key, texture]: textures) {
                delete texture;
            }
            for(auto& [key, mesh]: meshes) {
                delete mesh;
            }
        }
        static std::pair<std::string, std::string> canonicalize_shader(std::string vert_name, std::string frag_name) {
            auto& self = *ResourceManager::self;
            return {self.base_path + "shaders/" + vert_name, self.base_path + "shaders/" + frag_name};
        }
        static std::string canonicalize_texture(std::string name) {
            auto& self = *ResourceManager::self;
            return self.base_path + "textures/" + name;
        }
        static std::string canonicalize_mesh(std::string name) {
            auto& self = *ResourceManager::self;
            return self.base_path + "meshes/" + name;
        }
        static Shader& shader(std::string vert_name, std::string frag_name) {
            std::string shader_name = vert_name + ":" + frag_name;
            if(ResourceManager::self->shaders.find(shader_name)
            == ResourceManager::self->shaders.end()) {
                auto name = ResourceManager::self->canonicalize_shader(vert_name, frag_name);
                std::cout << "loading " << name.first << ", " << name.second << std::endl;
                ResourceManager::self->shaders[shader_name] = new Shader{name.first, name.second};
            }
            return *ResourceManager::self->shaders[shader_name];
        }
        static Texture& texture(std::string tex_name) {
            if(ResourceManager::self->textures.find(tex_name)
            == ResourceManager::self->textures.end()) {
                auto name = ResourceManager::self->canonicalize_texture(tex_name);
                std::cout << "loading " << name << std::endl;
                ResourceManager::self->textures[tex_name] = new Texture{name};
            }
            return *ResourceManager::self->textures[tex_name];
        }
        static Mesh& mesh(std::string mesh_name) {
            if(ResourceManager::self->meshes.find(mesh_name)
            == ResourceManager::self->meshes.end()) {
                auto name = ResourceManager::self->canonicalize_mesh(mesh_name);
                std::cout << "loading " << name << std::endl;
                ResourceManager::self->meshes[mesh_name] = new Mesh{name};
            }
            return *ResourceManager::self->meshes[mesh_name];
        }
};


#endif // __RESOURCE_MANAGER_H_
