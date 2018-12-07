#version 130

#ifdef GL_ES
precision mediump int;
precision mediump float;
#endif

in vec2 texcoord;
uniform vec2 u_resolution;
uniform sampler2D texture;

vec2 offsets[8] = vec2[8](
    vec2(-1, -1),
    vec2(-1, 0),
    vec2(-1, 1),
    vec2(0, -1),
    vec2(0, 1),
    vec2(1, -1),
    vec2(1, 0),
    vec2(1, 1)
);

vec2 offsets2[9] = vec2[9](
    vec2(-1.0f,  1.0f), // top-left
    vec2( 0.0f,    1.0f), // top-center
    vec2( 1.0f,  1.0f), // top-right
    vec2(-1.0f,  0.0f),   // center-left
    vec2( 0.0f,    0.0f),   // center-center
    vec2( 1.0f,  0.0f),   // center-right
    vec2(-1.0f, -1.0f), // bottom-left
    vec2( 0.0f,   -1.0f), // bottom-center
    vec2( 1.0f, -1.0f)  // bottom-right
);

float kernel[9] = float[](
    1.0 / 16, 2.0 / 16, 1.0 / 16,
    2.0 / 16, 4.0 / 16, 2.0 / 16,
    1.0 / 16, 2.0 / 16, 1.0 / 16
);

vec4 edgeColor = vec4(1,0,0,1);
float threshold = 0.1;

void main()
{
    vec4 neighboor = vec4(0);
    for (int i =0; i<8; i++) {
        neighboor += texture2D(texture, texcoord + offsets[i] / u_resolution);
    }
    neighboor/=8;
    vec4 col = texture2D(texture, texcoord);
    float s = step(threshold, length(col - neighboor));
    gl_FragColor = mix(col, col, s);
}


