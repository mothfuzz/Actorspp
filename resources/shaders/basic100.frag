#version 100

precision highp float;

//model
varying vec3 frag_position;
varying vec2 frag_texcoord;
varying vec3 frag_normal;
varying vec3 frag_tangent;

//point lights
struct light {
    vec3 color;
    vec3 position;
};

#define NUM_LIGHTS 8
uniform light lights[NUM_LIGHTS];
varying vec3 frag_light_direction[NUM_LIGHTS];

//sun light
uniform vec4 sun_color;
uniform vec3 sun_direction;
uniform sampler2D sun_shadows;
varying vec3 frag_sun_direction;
varying vec4 frag_position_sun_space;

//camera
varying vec3 cam_direction;

//textures
uniform sampler2D albedo_tex;
uniform sampler2D normal_tex;
uniform sampler2D glossy_tex;

float shadow() {
    vec3 coords = frag_position_sun_space.xyz / frag_position_sun_space.w;
    coords = coords * 0.5 + 0.5;
    float nearest = texture2D(sun_shadows, coords.xy).r;
    float depth = coords.z;
    float bias = max(0.01 * (1.0 - dot(frag_normal, sun_direction)), 0.001);
    if(depth - bias > nearest) {
        return 1.0;
    } else {
        return 0.0;
    }
}

vec4 calc_light(bool sun, vec4 color, vec3 direction, vec3 normal, vec3 eye, vec4 base_color, vec4 spec_color, float shininess) {
        vec3 n = normalize(normal);
        vec3 l = normalize(direction);

        vec3 h = reflect(-l, n);
        vec3 v = normalize(eye);

        //shade
        float ambient = color.a;
        float diffuse = clamp(dot(n, l), 0.0, 1.0);
        float specular = pow(clamp(dot(v, h), 0.0, 1.0), shininess);

        float sh = 1.0;
        if(sun) {
            sh -= shadow();
        }
        return vec4(color.rgb*(ambient+diffuse*sh), 1.0) * base_color +
                vec4(color.rgb*(specular*sh), 1.0) * spec_color;
}

void main() {

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

    vec3 normal = texture2D(normal_tex, frag_texcoord).xyz * 2.0 - 1.0; //0.0-1.0 -> -1.0-1.0

    vec4 base_color = texture2D(albedo_tex, frag_texcoord);
    vec4 output_color = calc_light(true, sun_color, frag_sun_direction, normal, cam_direction, base_color, spec_color, shininess);
    for(int i = 0; i < NUM_LIGHTS; i++) {
        output_color += calc_light(false, vec4(lights[i].color, 0.0), frag_light_direction[i], normal, cam_direction, base_color, spec_color, shininess);
    }
    gl_FragColor = output_color;

    //gl_FragColor = vec4(1.0-shadow(), 1.0-shadow(), 1.0-shadow(), 1.0);
}
