#ifndef __MESH_H_
#define __MESH_H_

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
//#include <array>

#include "gl.hpp"

class Mesh {
    //indexed vertex data
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> texcoords;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec3> tangents;
    //std::vector<glm::vec3> bitangents;
    GLuint id; //vao
    GLuint vbo_MVP;
    GLuint vbo_model;
    GLuint vbo_model_normal;
public:
        Mesh(std::string filename) {

            std::ifstream file(filename);
            if(!file.is_open()) {
                throw std::runtime_error("unable to open " + filename);
            }

            //raw vertex data
            std::vector<glm::vec3> v;
            std::vector<glm::vec2> vt;
            std::vector<glm::vec3> vn;

            std::string line;
            while(std::getline(file, line)) {
                std::stringstream line_ss(line);
                std::string data_entry;
                line_ss >> data_entry;
                if(data_entry == "v") {
                    float x, y, z;
                    line_ss >> x;
                    line_ss >> y;
                    line_ss >> z;
                    v.push_back(glm::vec3{x, y, z});
                }
                if(data_entry == "vt") {
                    float u, v;
                    line_ss >> u;
                    line_ss >> v;
                    //note: texcoords v-flipped bc OpenGL coord sys different from SDL_Image png loading
                    vt.push_back(glm::vec2{u, 1.0f - v});
                }
                if(data_entry == "vn") {
                    float x, y, z;
                    line_ss >> x;
                    line_ss >> y;
                    line_ss >> z;
                    vn.push_back(glm::vec3{x, y, z});
                }
                if(data_entry == "f") {
                    std::string v1, v2, v3;
                    line_ss >> v1;
                    line_ss >> v2;
                    line_ss >> v3;
                    int v1v, v1vt, v1vn,
                        v2v, v2vt, v2vn,
                        v3v, v3vt, v3vn;
                    std::stringstream v1_ss(v1);
                    std::stringstream v2_ss(v2);
                    std::stringstream v3_ss(v3);
                    char slash; //this is dumb

                    v1_ss >> v1v >> slash >> v1vt >> slash >> v1vn;
                    v2_ss >> v2v >> slash >> v2vt >> slash >> v2vn;
                    v3_ss >> v3v >> slash >> v3vt >> slash >> v3vn;

                    vertices.push_back(v[v1v-1]);
                    texcoords.push_back(vt[v1vt-1]);
                    normals.push_back(vn[v1vn-1]);

                    vertices.push_back(v[v2v-1]);
                    texcoords.push_back(vt[v2vt-1]);
                    normals.push_back(vn[v2vn-1]);

                    vertices.push_back(v[v3v-1]);
                    texcoords.push_back(vt[v3vt-1]);
                    normals.push_back(vn[v3vn-1]);

                    //calculate tangents
                    glm::vec3 dv1 = v[v2v-1] - v[v1v-1];
                    glm::vec3 dv2 = v[v3v-1] - v[v1v-1];
                    glm::vec2 duv1 = vt[v2vt-1] - vt[v1vt-1];
                    glm::vec2 duv2 = vt[v3vt-1] - vt[v1vt-1];
                    float r = 1.0f / (duv1.x * duv2.y - duv2.x * duv1.y);
                    glm::vec3 tangent = (dv1 * duv2.y - dv2 * duv1.y)*r;
                    //glm::vec3 bitangent = (dv2 * duv1.x - dv1 * duv2.x)*r;
                    tangents.push_back(tangent); //v1
                    tangents.push_back(tangent); //v2
                    tangents.push_back(tangent); //v3
                }
            }


            glGenVertexArrays(1, &id);
            glBindVertexArray(id);

            GLuint vertices_vbo;
            glGenBuffers(1, &vertices_vbo);
            glBindBuffer(GL_ARRAY_BUFFER, vertices_vbo);
            glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);

            GLuint texcoords_vbo;
            glGenBuffers(1, &texcoords_vbo);
            glBindBuffer(GL_ARRAY_BUFFER, texcoords_vbo);
            glBufferData(GL_ARRAY_BUFFER, texcoords.size()*sizeof(glm::vec2), texcoords.data(), GL_STATIC_DRAW);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 2, GL_FLOAT, false, 0, 0);

            GLuint normals_vbo;
            glGenBuffers(1, &normals_vbo);
            glBindBuffer(GL_ARRAY_BUFFER, normals_vbo);
            glBufferData(GL_ARRAY_BUFFER, normals.size()*sizeof(glm::vec3), normals.data(), GL_STATIC_DRAW);
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 3, GL_FLOAT, false, 0, 0);

            GLuint tangents_vbo;
            glGenBuffers(1, &tangents_vbo);
            glBindBuffer(GL_ARRAY_BUFFER, tangents_vbo);
            glBufferData(GL_ARRAY_BUFFER, tangents.size()*sizeof(glm::vec3), tangents.data(), GL_STATIC_DRAW);
            glEnableVertexAttribArray(3);
            glVertexAttribPointer(3, 3, GL_FLOAT, false, 0, 0);

            glGenBuffers(1, &vbo_MVP);
            glBindBuffer(GL_ARRAY_BUFFER, vbo_MVP);
            glEnableVertexAttribArray(5);
            glVertexAttribPointer(5, 4, GL_FLOAT, false, 4 * sizeof(glm::vec4), (void*)(0 * sizeof(glm::vec4)));
            glVertexAttribDivisor(5, 1);
            glEnableVertexAttribArray(6);
            glVertexAttribPointer(6, 4, GL_FLOAT, false, 4 * sizeof(glm::vec4), (void*)(1 * sizeof(glm::vec4)));
            glVertexAttribDivisor(6, 1);
            glEnableVertexAttribArray(7);
            glVertexAttribPointer(7, 4, GL_FLOAT, false, 4 * sizeof(glm::vec4), (void*)(2 * sizeof(glm::vec4)));
            glVertexAttribDivisor(7, 1);
            glEnableVertexAttribArray(8);
            glVertexAttribPointer(8, 4, GL_FLOAT, false, 4 * sizeof(glm::vec4), (void*)(3 * sizeof(glm::vec4)));
            glVertexAttribDivisor(8, 1);

            glGenBuffers(1, &vbo_model);
            glBindBuffer(GL_ARRAY_BUFFER, vbo_model);
            glEnableVertexAttribArray(9);
            glVertexAttribPointer(9, 4, GL_FLOAT, false, 4 * sizeof(glm::vec4), (void*)(0 * sizeof(glm::vec4)));
            glVertexAttribDivisor(9, 1);
            glEnableVertexAttribArray(10);
            glVertexAttribPointer(10, 4, GL_FLOAT, false, 4 * sizeof(glm::vec4), (void*)(1 * sizeof(glm::vec4)));
            glVertexAttribDivisor(10, 1);
            glEnableVertexAttribArray(11);
            glVertexAttribPointer(11, 4, GL_FLOAT, false, 4 * sizeof(glm::vec4), (void*)(2 * sizeof(glm::vec4)));
            glVertexAttribDivisor(11, 1);
            glEnableVertexAttribArray(12);
            glVertexAttribPointer(12, 4, GL_FLOAT, false, 4 * sizeof(glm::vec4), (void*)(3 * sizeof(glm::vec4)));
            glVertexAttribDivisor(12, 1);

            glGenBuffers(1, &vbo_model_normal);
            glBindBuffer(GL_ARRAY_BUFFER, vbo_model_normal);
            glEnableVertexAttribArray(13);
            glVertexAttribPointer(13, 3, GL_FLOAT, false, 3 * sizeof(glm::vec3), (void*)(0 * sizeof(glm::vec3)));
            glVertexAttribDivisor(13, 1);
            glEnableVertexAttribArray(14);
            glVertexAttribPointer(14, 3, GL_FLOAT, false, 3 * sizeof(glm::vec3), (void*)(1 * sizeof(glm::vec3)));
            glVertexAttribDivisor(14, 1);
            glEnableVertexAttribArray(15);
            glVertexAttribPointer(15, 3, GL_FLOAT, false, 3 * sizeof(glm::vec3), (void*)(2 * sizeof(glm::vec3)));
            glVertexAttribDivisor(15, 1);

            glBindVertexArray(0);
        }
        void bind() {
            glBindVertexArray(id);
        }
        void unbind() {
            glBindVertexArray(0);
        }
        void draw_instanced(std::vector<glm::mat4>& models, glm::mat4& view, glm::mat4& proj) {
            std::vector<glm::mat4> MVPs;
            std::vector<glm::mat3> model_normals;
            for (glm::mat4& model : models) {
                MVPs.push_back(proj * view * model);
                model_normals.push_back(glm::mat3(glm::transpose(glm::inverse(model))));
            }
            glBindBuffer(GL_ARRAY_BUFFER, vbo_MVP);
            glBufferData(GL_ARRAY_BUFFER, MVPs.size() * sizeof(glm::mat4), MVPs.data(), GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, vbo_model);
            glBufferData(GL_ARRAY_BUFFER, models.size() * sizeof(glm::mat4), models.data(), GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, vbo_model_normal);
            glBufferData(GL_ARRAY_BUFFER, model_normals.size() * sizeof(glm::mat3), model_normals.data(), GL_DYNAMIC_DRAW);
            draw(models.size());
        }
        void draw(int count=1) {
            glBindVertexArray(id);
            //glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
            //glDrawArrays(GL_TRIANGLES, 0, vertices.size());
            glDrawArraysInstanced(GL_TRIANGLES, 0, vertices.size(), count);
            glBindVertexArray(0);
        }
};

#endif // __MESH_H_
