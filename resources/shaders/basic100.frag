#version 100

precision highp float;

varying vec3 frag_position;
varying vec2 frag_texcoord;
varying vec3 frag_normal;
varying vec3 frag_tangent;
//varying mat3 TBN;

varying vec3 light_direction;
varying vec3 cam_direction;

uniform sampler2D albedo_tex;
uniform sampler2D normal_tex;
uniform sampler2D glossy_tex;

uniform bool reflective;


void main() {

    vec3 light_color = vec3(1, 1, 1);

    //can have specular, shininess, and iridescence in one texture! shininess = grayscale average of specular color.
    vec4 gloss = texture2D(glossy_tex, frag_texcoord);
    vec4 spec_color = gloss;
    float shininess = gloss.r + gloss.g + gloss.b;
    if(gloss.r == gloss.g && gloss.g == gloss.b) {
        shininess = 127.0 * (shininess / 3.0) + 1.0;
    } else {
        //iridescent materials have to have larger diameters.
        shininess = 7.0 * (shininess / 3.0) + 1.0;
    }

    //vec3 n = frag_normal;
    //vec3 l = light_direction;

    vec3 n = normalize(texture2D(normal_tex, frag_texcoord).xyz * 2.0 - 1.0); //0.0-1.0 -> -1.0-1.0
    vec3 l = normalize(light_direction);

    vec3 h = reflect(-l, n);
    vec3 v = normalize(cam_direction);

    //shade
    float ambient = 0.3;
    float diffuse = clamp(dot(n, l), 0.0, 1.0);
    float specular = pow(clamp(dot(v, h), 0.0, 1.0), shininess);

    if(reflective) {
        vec4 base_color = texture2D(albedo_tex, clamp(frag_texcoord+n.rg/64.0, 0.0, 1.0));
        gl_FragColor = base_color;//vec4(light_color*(ambient+diffuse), 1.0) * base_color;
    } else {
        vec4 base_color = texture2D(albedo_tex, frag_texcoord);
        gl_FragColor = vec4(light_color*(ambient+diffuse), 1.0) * base_color +
                vec4(light_color*(specular), 1.0) * spec_color;
    }

    //visualize normals
    //gl_FragColor = vec4((transpose(TBN) * n + 1.0)/2.0, 1.0);
}
