#version 130

#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

in vec2 v_texcoord;
in vec3 v_normal;

float near = 1.0;
float far  = 100.0;

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

void main()
{
    float depth = LinearizeDepth(gl_FragCoord.z) / 100.0; // divide by far for demonstration
    float d = map(depth, 1.0, 0.0, 0.0,1.0);

    vec3 v_normal = map(v_normal, -1.0,1.0, 0.0,1.0);
    vec4 normalColor = vec4(v_normal,1.0);
    normalColor.b = d;

    gl_FragColor = normalColor;
}
