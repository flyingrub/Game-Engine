#version 130
in vec2 texcoord;
uniform sampler2D texture;
uniform sampler2D bloomTexture;
uniform float exposure;

void main()
{
    const float gamma = 2.2;
    vec3 hdrColor = texture2D(bloomTexture, texcoord).rgb;


    // reinhard tone mapping
    vec3 mapped = hdrColor / (hdrColor + vec3(1.0));

    // Exposure tone mapping
    //mapped = vec3(1.0) - exp(-hdrColor * exposure);
    // gamma correction
    mapped = pow(mapped, vec3(1.0 / gamma));

    gl_FragColor = vec4(mapped, 1.0);
}
