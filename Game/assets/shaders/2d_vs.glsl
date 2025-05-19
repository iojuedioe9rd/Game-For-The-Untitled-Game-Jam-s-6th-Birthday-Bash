#version 300 es
precision mediump float;

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;

uniform mat4 u_MVP;
uniform vec4 u_Color;

out vec2 v_TexCoord;
out vec4 v_Color;

void main() {
    v_TexCoord = a_TexCoord;
    v_Color    = u_Color;
    gl_Position = u_MVP * vec4(a_Position, 1.0);
}
