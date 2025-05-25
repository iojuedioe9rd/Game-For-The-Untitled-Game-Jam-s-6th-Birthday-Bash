#version 300 es
precision mediump float;

layout (location = 0) in vec2 inPos;
layout (location = 1) in vec2 inTexCoords;

out vec2 pos;

void main()
{
    pos = inPos; 
    gl_Position = vec4(pos, 0.0, 1.0); 
}  