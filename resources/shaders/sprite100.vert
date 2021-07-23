#version 100

attribute vec3 vertex;
attribute vec4 clip_rect;
attribute mat4 MVP;

varying vec2 frag_texcoord;

void main() {
    gl_Position = MVP * vec4(vertex, 1.0);

    float u = vertex.x + 0.5;
    float v = -vertex.y + 0.5;
    if(u == 0.0) {
        u = clip_rect[0];
    }
    if(u == 1.0) {
        u = clip_rect[0]+clip_rect[2];
    }
    if(v == 0.0) {
        v = clip_rect[1];
    }
    if(v == 1.0) {
        v = clip_rect[1]+clip_rect[3];
    }
    frag_texcoord = vec2(u, v);
}
