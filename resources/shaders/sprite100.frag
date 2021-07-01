#version 100

precision highp float;

varying vec2 frag_texcoord;

uniform sampler2D sprite;

void main() {
    gl_FragColor = texture2D(sprite, frag_texcoord);
    //so we don't have to enable blending lol
    if(gl_FragColor.a == 0.0) {
        discard;
    }
    //color key
    if(gl_FragColor.rgb == vec3(1, 0, 1)) {
        discard;
    }
}
