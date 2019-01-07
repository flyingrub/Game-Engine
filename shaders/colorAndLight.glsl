#version 130
#extension GL_ARB_explicit_attrib_location : require

#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

struct Light {
    vec3 position;
    vec3 color;
    bool isDir;
    bool isActive;

    float constant;
    float linear;
    float quadratic;

};

const int lightsNumber = 4;
uniform Light lights[lightsNumber];
uniform sampler2D texture;
uniform float time;
uniform float brightThreshold;


in vec2 v_texcoord;
in vec3 v_normal;
in vec3 v_frag_pos;

in float altitude;

float near = 1.0;
float far  = 100.0;

layout(location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // back to NDC
    return (2.0 * near * far) / (far + near - z * (far - near));
}

vec3 map(vec3 value, float inMin, float inMax, float outMin, float outMax) {
  return outMin + (outMax - outMin) * (value - inMin) / (inMax - inMin);
}

float map(float value, float inMin, float inMax, float outMin, float outMax) {
  return outMin + (outMax - outMin) * (value - inMin) / (inMax - inMin);
}

vec4 calcDirLight(Light light, vec3 normal)
{
    vec3 lightDir = normalize(-light.position);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff*light.color;
    return vec4(diffuse, 1.0);
}

vec4 calcPointLight(Light light, vec3 normal)
{
    vec3 lightDir = normalize(light.position - v_frag_pos);
    float diff = max(dot(normal, lightDir), 0.0);
    float distance    = length(light.position - v_frag_pos);
    float attenuation = 1.0 / (light.constant + light.linear * distance +
                        light.quadratic * (distance * distance));
    vec3 ambient = light.color * attenuation * 0.2;
    vec3 diffuse = diff * light.color * attenuation + ambient;
    return vec4(diffuse, 1.0);
}

void main()
{
    float depth = LinearizeDepth(gl_FragCoord.z) / 100.0; // divide by far for demonstration
    float d = map(depth, 1.0, 0.0, 0.0,1.0);

    vec4 textureColor = texture2D(texture, v_texcoord);
    vec4 depthColor = vec4(vec3(depth), 1.0);
    vec4 altitudeColor = vec4(vec3(altitude), 1.0);

    vec3 n = normalize(v_normal);
    vec4 light_color = vec4(0);
    for(int i = 0; i < lightsNumber; i++) {
        Light l = lights[i];
        if(!l.isActive) {continue;}
        if (l.isDir) {
            light_color += calcDirLight(l, n);
        } else {
            light_color += calcPointLight(l, n);
        }
    }

    FragColor = textureColor * light_color;
    float brightness = dot(FragColor.rgb, vec3(brightThreshold));
    if(brightness > 1.0)
        BrightColor = vec4(FragColor.rgb, 1.0);
    else
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}
