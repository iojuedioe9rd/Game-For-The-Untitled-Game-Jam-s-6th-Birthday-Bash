// assets/shaders/2d_vs.glsl
#version 300 es
precision mediump float;

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in float a_TexID;   // now a float
layout(location = 3) in vec2  a_TexCoord;

uniform mat4 u_ViewProjection;

flat   out int   v_TexID;    // pass an integer downstream
       out vec2  v_TexCoord;
       out vec4  v_Color;

void main() {
    v_TexCoord = a_TexCoord;
    v_Color    = a_Color;
    // cast float -> int for flat interpolation
    v_TexID    = int(a_TexID);
    gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
}
