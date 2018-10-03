#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

uniform sampler2D texture;

varying vec2 v_texcoord;
varying float z;

float near = 0.1;
float far  = 10.0;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // back to NDC
    return (2.0 * near * far) / (far + near - z * (far - near));
}

void main()
{
    float depth = LinearizeDepth(gl_FragCoord.z) / far; // divide by far for demonstration
    float alt = z / 2.0;

    // gl_FragColor = texture2D(texture, v_texcoord); // Texture
    gl_FragColor = vec4(vec3(depth), 1.0); // Depth
    // gl_FragColor = vec4(vec3(alt), 1.0); // Altitude
}

//! [0]

