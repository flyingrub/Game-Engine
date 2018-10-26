#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif
uniform sampler2D texture;


varying vec2 v_texcoord;
varying float altitude;
uniform float time;

uniform vec3 light_pos;
uniform vec3 light_color;



float near = 0.1;
float far  = 15.0;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // back to NDC
    return (2.0 * near * far) / (far + near - z * (far - near));
}

void main()
{
    float depth = LinearizeDepth(gl_FragCoord.z) / far; // divide by far for demonstration

    vec4 textureColor = texture2D(texture, v_texcoord);
    vec4 depthColor = vec4(vec3(depth), 1.0);
    vec4 altitudeColor = vec4(vec3(altitude), 1.0);

    vec4 color = textureColor * altitudeColor;
    gl_FragColor = textureColor;
}

//! [0]

