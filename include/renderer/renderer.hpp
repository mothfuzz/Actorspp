#ifndef __RENDERER_H_
#define __RENDERER_H_

#include "resource_manager.hpp"
#include "window.hpp"
#include "mesh.hpp"
#include "shader.hpp"
#include "texture.hpp"

#include "components.hpp"


struct Camera {};

struct Light {
    glm::vec3 color = { 1.0f, 1.0f, 1.0f };
};

struct Transform {
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale = {1.0f, 1.0f, 1.0f};
};
#define TRANSFORM_ORIGIN {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}}

struct Model {
    std::string mesh;
};

struct Sprite {
    std::string texture;
    float width;
    float height;
    struct ClipRect {
        float x;
        float y;
        float w;
        float h;
    } clip_rect;
};

#define DEFAULT_VERTEX_SHADER "basic100.vert"
#define DEFAULT_FRAGMENT_SHADER "basic100.frag"
#define DEFAULT_SHADER DEFAULT_VERTEX_SHADER, DEFAULT_FRAGMENT_SHADER
#define ALBEDO_TEX "albedo_tex"
#define NORMAL_TEX "normal_tex"
#define GLOSSY_TEX "glossy_tex"
#define DEFAULT_SPRITE_SHADER "sprite100.vert", "sprite100.frag"
struct Material {
    std::string vertex_shader = "basic100.vert";
    std::string fragment_shader = "basic100.frag";
    std::unordered_map<std::string, std::string> textures;
};

//Kay's wonderful auto-batching and auto-instancing 2D & 3D combined renderer
class Renderer {
    int active_camera_id = 0;
    glm::mat4 persp;
    glm::mat4 ortho;
    float z_2D = 0.0f;
    inline static GLuint sprite_vao = 0;
    inline static GLuint sprite_vbo_clip_rect = 0;
    inline static GLuint sprite_vbo_mvp = 0;
    public:
        Renderer() {
            float width = static_cast<float>(Window::get_width());
            float height = static_cast<float>(Window::get_height());
            persp = glm::perspective(glm::radians(60.0f), width/height, 0.1f, height*2.0f);
            ortho = glm::ortho(0.0f, width, height, 0.0f, -100.0f, 100.0f);
            //ortho = persp;
            if(z_2D == 0.0f) {
                z_2D = sqrt(pow(Window::get_height(), 2) - pow(Window::get_height()/2.0f, 2));
            }
            if(Renderer::sprite_vao == 0) {
                glGenVertexArrays(1, &sprite_vao);
                glBindVertexArray(sprite_vao);
                std::vector<glm::vec3> vertices = {{-0.5f, -0.5f, 0.0f}, {0.5f, -0.5f, 0.0f}, {-0.5f, 0.5f, 0.0f},
                                                   {0.5f, -0.5f, 0.0f}, {0.5f, 0.5f, 0.0f}, {-0.5f, 0.5f, 0.0f}};
                GLuint vbo;
                glGenBuffers(1, &vbo);
                glBindBuffer(GL_ARRAY_BUFFER, vbo);
                glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);
                glEnableVertexAttribArray(0);
                glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);

                glGenBuffers(1, &sprite_vbo_clip_rect);
                glBindBuffer(GL_ARRAY_BUFFER, sprite_vbo_clip_rect);
                glEnableVertexAttribArray(1);
                glVertexAttribPointer(1, 4, GL_FLOAT, false, sizeof(glm::vec4), 0);
                glVertexAttribDivisor(1, 1);

                glGenBuffers(1, &sprite_vbo_mvp);
                glBindBuffer(GL_ARRAY_BUFFER, sprite_vbo_mvp);
                glEnableVertexAttribArray(2);
                glVertexAttribPointer(2, 4, GL_FLOAT, false, 4*sizeof(glm::vec4), (void*)(0 * sizeof(glm::vec4)));
                glVertexAttribDivisor(2, 1);
                glEnableVertexAttribArray(3);
                glVertexAttribPointer(3, 4, GL_FLOAT, false, 4*sizeof(glm::vec4), (void*)(1 * sizeof(glm::vec4)));
                glVertexAttribDivisor(3, 1);
                glEnableVertexAttribArray(4);
                glVertexAttribPointer(4, 4, GL_FLOAT, false, 4*sizeof(glm::vec4), (void*)(2 * sizeof(glm::vec4)));
                glVertexAttribDivisor(4, 1);
                glEnableVertexAttribArray(5);
                glVertexAttribPointer(5, 4, GL_FLOAT, false, 4*sizeof(glm::vec4), (void*)(3 * sizeof(glm::vec4)));
                glVertexAttribDivisor(5, 1);

                glBindVertexArray(0);
            }
        }

        //returns mvp and clip_rect
        std::pair<glm::mat4, glm::vec4> prepare_sprite_transform(Sprite* s, Transform* t, glm::mat4& view, glm::mat4& proj, int tex_w, int tex_h, bool use_ortho=false) {
            glm::mat4 model = glm::identity<glm::mat4>();
            if (t) {
                model = glm::translate(model, t->position);
            }
            glm::mat4 mv = view * model;
            if (!use_ortho) {
                //sprites are billboarded
                mv[0][0] = 1;
                mv[0][1] = 0;
                mv[0][2] = 0;
                mv[1][0] = 0;
                mv[1][1] = 1;
                mv[1][2] = 0;
                mv[2][0] = 0;
                mv[2][1] = 0;
                mv[2][2] = 1;
            }
            if (t) {
                mv = glm::rotate(mv, glm::radians(t->rotation.x), glm::vec3{ 1.0f, 0.0f, 0.0f });
                mv = glm::rotate(mv, glm::radians(t->rotation.y), glm::vec3{ 0.0f, 1.0f, 0.0f });
                mv = glm::rotate(mv, glm::radians(t->rotation.z), glm::vec3{ 0.0f, 0.0f, 1.0f });
                mv = glm::scale(mv, t->scale);
            }

            //default to texture size if w/h not specified manually
            if (s->width == 0.0f) {
                s->width = tex_w;
            }
            if (s->height == 0.0f) {
                s->height = tex_h;
            }
            mv = glm::scale(mv, glm::vec3(s->width, s->height, 1.0f));

            if (s->clip_rect.w == 0.0f) {
                s->clip_rect.w = tex_w;
            }
            if (s->clip_rect.h == 0.0f) {
                s->clip_rect.h = tex_h;
            }
            //conv clip_rect from sprite space(0,0 - w,h) to tex space(0,0, - 1,1)
            glm::vec4 clip_rect = { s->clip_rect.x / tex_w, s->clip_rect.y / tex_h, s->clip_rect.w / tex_w, s->clip_rect.h / tex_h };

            glm::mat4 mvp = proj * mv;

            return { mvp, clip_rect };
        }

        void draw_sprite_instanced(std::vector<int>& ids, int tex_w, int tex_h, Shader& shader, glm::mat4& view, glm::mat4& proj, bool use_ortho = false) {
            std::vector<glm::mat4> mvps;
            std::vector<glm::vec4> clip_rects;
            for (int id : ids) {
                Sprite* s = Component<Sprite>::entry(id);
                Transform* t = Component<Transform>::entry(id);
                auto p = prepare_sprite_transform(s, t, view, proj, tex_w, tex_h, use_ortho);
                mvps.push_back(p.first);
                clip_rects.push_back(p.second);
            }
            glBindBuffer(GL_ARRAY_BUFFER, sprite_vbo_clip_rect);
            glBufferData(GL_ARRAY_BUFFER, clip_rects.size() * sizeof(glm::vec4), clip_rects.data(), GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, sprite_vbo_mvp);
            glBufferData(GL_ARRAY_BUFFER, mvps.size() * sizeof(glm::mat4), mvps.data(), GL_DYNAMIC_DRAW);
            glDrawArraysInstanced(GL_TRIANGLES, 0, 6, ids.size());
        }

        void render_sprites(glm::mat4& view, glm::mat4& proj, bool use_ortho=false) {

            std::unordered_map<std::string, std::vector<int>> normal_sprites; //sorted by texture
            std::vector<int> material_sprites; //have special materials, require extra state changes

            for (int id : Component<Sprite>::keys()) {
                Sprite* s = Component<Sprite>::entry(id);
                if (Material* m = Component<Material>::entry(id)) {
                    material_sprites.push_back(id);
                }
                else {
                    normal_sprites[s->texture].push_back(id);
                }
            }

            glBindVertexArray(sprite_vao);
            for (auto& [tex_name, sprites] : normal_sprites) {
                Texture& tex = ResourceManager::texture(tex_name);
                float tex_w = tex.get_width();
                float tex_h = tex.get_height();
                Shader& shader = ResourceManager::shader(DEFAULT_SPRITE_SHADER);
                shader.bind();
                shader.attach_texture("sprite", tex);
                draw_sprite_instanced(sprites, tex_w, tex_h, shader, view, proj);
                //shader.unbind();
            }
            for (int id : material_sprites) {
                Sprite* s = Component<Sprite>::entry(id);
                Texture& tex = ResourceManager::texture(s->texture);
                float tex_w = tex.get_width();
                float tex_h = tex.get_height();
                Material* mat = Component<Material>::entry(id);
                Shader& shader = ResourceManager::shader(mat->vertex_shader, mat->fragment_shader);
                shader.bind();
                shader.attach_texture("sprite", tex);
                for (auto& [uniform_name, texture_id] : mat->textures) {
                    //std::cout << "attaching " << texture_id << " to " << uniform_name << " in " << mat->fragment_shader << std::endl;
                    shader.attach_texture(uniform_name, ResourceManager::texture(texture_id));
                }
                std::vector<int> ids = { id };
                draw_sprite_instanced(ids, tex_w, tex_h, shader, view, proj);
                //shader.unbind();
            }
            //glBindVertexArray(0);
        }

        glm::mat4 prepare_model_transform(Transform* t) {
            glm::mat4 model = glm::identity<glm::mat4>();
            if (t) {
                model = glm::translate(model, t->position);
                model = glm::rotate(model, glm::radians(t->rotation.x), glm::vec3{ 1.0f, 0.0f, 0.0f });
                model = glm::rotate(model, glm::radians(t->rotation.y), glm::vec3{ 0.0f, 1.0f, 0.0f });
                model = glm::rotate(model, glm::radians(t->rotation.z), glm::vec3{ 0.0f, 0.0f, 1.0f });
                model = glm::scale(model, t->scale);
            }
            return model;
        }

        void render_models(glm::mat4& view, glm::mat4& proj, glm::vec3& camera_pos) {
            std::map<std::tuple<std::string, std::string, std::string, std::string>, std::vector<int>> normal_models; //hashed by mesh_id+albedo+normal+glossy
            std::vector<int> material_models;
            for (int id : Component<Model>::keys()) {
                Model* m = Component<Model>::entry(id);
                Material* mat = Component<Material>::entry(id);
                if (mat->vertex_shader == DEFAULT_VERTEX_SHADER && mat->fragment_shader == DEFAULT_FRAGMENT_SHADER) {
                    normal_models[{m->mesh, mat->textures[ALBEDO_TEX], mat->textures[NORMAL_TEX], mat->textures[GLOSSY_TEX]}].push_back(id);
                }
                else {
                    material_models.push_back(id);
                }
            }
            for (auto& [mat_tup, ids] : normal_models) {
                auto& [mesh, albedo_tex, normal_tex, glossy_tex] = mat_tup;
                Shader& shader = ResourceManager::shader(DEFAULT_SHADER);
                shader.bind();
                shader.attach_texture(ALBEDO_TEX, ResourceManager::texture(albedo_tex));
                shader.attach_texture(NORMAL_TEX, ResourceManager::texture(normal_tex));
                shader.attach_texture(GLOSSY_TEX, ResourceManager::texture(glossy_tex));
                shader.set_uniform("camera_position", camera_pos);
                //shader.set_uniform("reflective", true);

                std::vector<glm::mat4> MVPs;
                std::vector<glm::mat4> models;
                std::vector<glm::mat3> model_normals;
                for (int id : ids) {
                    Transform* t = Component<Transform>::entry(id);
                    models.push_back(prepare_model_transform(t));
                }

                //std::cout << "drawing " << models.size() << " of " << mesh << std::endl;
                ResourceManager::mesh(mesh).draw_instanced(models, view, proj);

                //shader.unbind();
            }

            for (int id : material_models) {
                Model* m = Component<Model>::entry(id);
                Transform* t = Component<Transform>::entry(id);

                Shader& shader = ResourceManager::shader(DEFAULT_SHADER);
                if (Material* mat = Component<Material>::entry(id)) {
                    shader = ResourceManager::shader(mat->vertex_shader, mat->fragment_shader);
                    shader.bind();
                    for (auto& [uniform_name, texture_id] : mat->textures) {
                        //std::cout << "attaching " << texture_id << " to " << uniform_name << " in " << mat->fragment_shader << std::endl;
                        shader.attach_texture(uniform_name, ResourceManager::texture(texture_id));
                    }
                }
                else {
                    shader.bind();
                }

                std::vector<glm::mat4> model = { prepare_model_transform(t) };
                ResourceManager::mesh(m->mesh).draw_instanced(model, view, proj);

                //shader.unbind();
            }
        }

        void render_all() {
            glm::mat4 view;
            glm::mat4 proj;

            Transform default_camera_t = {{0.0f, 0.0f, z_2D}, {0.0, 0.0, 0.0f}, {1.0f, 1.0f, 1.0f}};
            Transform *camera_t = nullptr;
            if(!active_camera_id || !(camera_t = Component<Transform>::entry(active_camera_id))) {
                camera_t = &default_camera_t;
            }

            glm::vec3 camera_pos = camera_t->position;
            bool use_ortho = false;
            if(use_ortho) {
                view = glm::identity<glm::mat4>();
                view = glm::translate(view, glm::vec3(-camera_pos.x, -camera_pos.y, 0.0f));
                //view = glm::scale(view, glm::vec3(2.0f, 2.0f, 1.0f)); //zoom factor
                proj = ortho;
            } else {
                view = glm::lookAt(camera_pos, camera_pos + glm::vec3{1.0f * sin(camera_t->rotation.y), 0.0f, -1.0f*cos(camera_t->rotation.y)},
                                glm::vec3{0.0f, 1.0f, 0.0f});
                proj = persp;
            }

            render_models(view, proj, camera_pos);
            render_sprites(view, proj, use_ortho);
        }
};

#endif // __RENDERER_H_
