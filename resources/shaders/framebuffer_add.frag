#version 100

precision highp float;

uniform sampler2D framebuffer_color_a;
uniform sampler2D framebuffer_color_b;

varying vec2 frag_texcoord;

void main() {
    vec4 color_a = texture2D(framebuffer_color_a, frag_texcoord);
    vec4 color_b = texture2D(framebuffer_color_b, frag_texcoord);
    gl_FragColor = color_a + color_b;
}
