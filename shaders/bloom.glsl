#version 130
#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif
uniform float threshold;

void main() {
    gl_FragColor = vec4(currentLightColor, 1.0);
}
