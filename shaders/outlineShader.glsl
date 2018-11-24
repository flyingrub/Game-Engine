#version 130

#ifdef GL_ES
precision mediump int;
precision mediump float;
#endif

in vec2 texcoord;
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

void main()
{
   vec4 neighboor;
   for (int i =0; i<8; i++) {
    //  neighboor +=
   }
    vec3 col = texture2D(texture, texcoord).rgb;
    gl_FragColor = vec4(col, 1.0);
}


