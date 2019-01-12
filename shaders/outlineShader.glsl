#version 130

#ifdef GL_ES
precision mediump int;
precision mediump float;
#endif

in vec2 texcoord;
uniform vec2 u_resolution;
uniform sampler2D texture;

vec2 offsets[8] = vec2[8](
    vec2(-1, 0),
    vec2(1, 0),
    vec2(0, -1),
    vec2(0, 1),
    vec2(-1, -1),
    vec2(-1, 1),
    vec2(1, -1),
    vec2(1, 1)
);

uniform vec3 edgeColor;
uniform float threshold;

void main()
{
    vec4 neighboor = vec4(0);
    for (int i =0; i<4; i++) {
        neighboor += texture2D(texture, texcoord + offsets[i] / u_resolution);
    }
    neighboor/=4;
    vec4 col = texture2D(texture, texcoord);
    // float s = step(threshold, length(col - neighboor));
    // gl_FragColor = mix(col, col, s);
    if (length(col - neighboor) < threshold) discard;
    gl_FragColor = vec4(edgeColor,1);
}


