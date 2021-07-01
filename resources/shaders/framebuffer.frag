#version 100

precision highp float;

uniform sampler2D framebuffer_color;
uniform sampler2D framebuffer_depth;

varying vec2 frag_texcoord;

uniform bool grayscale;

void main() {
    float bias = 0.0;
    vec3 color = texture2D(framebuffer_color, frag_texcoord, bias).rgb;
    if(grayscale) {
        float avg = (color.r + color.g + color.b)/3.0;
        color = vec3(avg, avg, avg);
    }

    gl_FragColor = vec4(color, 0.0);
    //gl_FragColor = texture2D(framebuffer_depth, frag_texcoord);
}
