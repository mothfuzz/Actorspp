#version 100

attribute vec3 vertex;
attribute vec2 texcoord;
attribute vec3 normal;
attribute vec3 tangent;

attribute mat4 MVP;
attribute mat4 model;
attribute mat3 model_normal;

varying vec3 frag_position;
varying vec2 frag_texcoord;
varying vec3 frag_normal;
varying vec3 frag_tangent;
varying vec3 frag_bitangent;

struct light {
    vec3 color;
    vec3 position;
};

#define NUM_LIGHTS 8
uniform light lights[NUM_LIGHTS];
varying vec3 frag_light_direction[NUM_LIGHTS];


uniform mat4 sun_transform;
uniform vec4 sun_color; //alpha = ambient intensity
uniform vec3 sun_direction;
varying vec3 frag_sun_direction;
varying vec4 frag_position_sun_space;

uniform vec3 camera_position;
varying vec3 cam_direction;

void main() {
    gl_Position = MVP * vec4(vertex, 1.0);
    frag_position = vec3(model * vec4(vertex, 1.0));
    frag_texcoord = texcoord;
    //mat3 model_normal = mat3(transpose(inverse(model)));

    frag_position_sun_space = sun_transform * vec4(frag_position, 1.0);

    //calculate TBN matrix...

    frag_tangent = normalize(model_normal * tangent);
    frag_normal = normalize(model_normal * normal);
    frag_tangent = normalize(frag_tangent - dot(frag_tangent, frag_normal) * frag_normal); //re-orthogonalize
    frag_bitangent = cross(frag_normal, frag_tangent);

    mat3 basis = mat3(frag_tangent, frag_bitangent, frag_normal);
    mat3 TBN = mat3(vec3(basis[0].x, basis[1].x, basis[2].x),
               vec3(basis[0].y, basis[1].y, basis[2].y),
               vec3(basis[0].z, basis[1].z, basis[2].z));

    //convert everything to tangent space
    frag_sun_direction = TBN * sun_direction;
    for(int i = 0; i < NUM_LIGHTS; i++) {
        frag_light_direction[i] = TBN * (lights[i].position - frag_position);
    }
    cam_direction = TBN * (camera_position - frag_position);
    frag_position = TBN * frag_position;
}
