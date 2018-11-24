#version 130
#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

uniform mat4 projection;
uniform mat4 matrix;

in vec4 a_position;
in vec2 a_texcoord;
in vec3 a_normal;

out vec2 v_texcoord;
out vec3 v_normal;
out float altitude;

//! [0]
void main()
{
    // Calculate vertex position in screen space
    gl_Position = projection * matrix * a_position;

    // Pass texture coordinate to fragment shader
    // Value will be automatically interpolated to fragments inside polygon faces
    v_texcoord = a_texcoord;
    v_normal = a_normal;
    altitude = a_position.z / 2.0;
}
//! [0]
