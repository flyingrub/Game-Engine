#version 130
in vec2 texcoord;
uniform vec2 u_resolution;
uniform sampler2D texture;
uniform float exposure;

void main()
{
    const float gamma = 2.2;
    vec3 hdrColor = texture2D(texture, texcoord).rgb;


    vec3 mapped = vec3(1.0) - exp(-hdrColor * exposure);

    mapped = pow(mapped, vec3(1.0 / gamma));

    //gl_FragColor = vec4(mapped, 1.0);
    gl_FragColor = vec4(1,0,0,1);
}
