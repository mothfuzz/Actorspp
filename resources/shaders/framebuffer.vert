#version 100

attribute vec3 vertex;

varying vec2 frag_texcoord;

void main() {
    gl_Position = vec4(vertex, 1.0);
    frag_texcoord = vec2((vertex.x + 1.0) / 2.0, (vertex.y + 1.0) / 2.0);
}
