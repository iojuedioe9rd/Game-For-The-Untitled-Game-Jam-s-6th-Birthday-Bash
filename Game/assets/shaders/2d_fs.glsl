#version 300 es
precision mediump float;

in vec2 v_TexCoord;
in vec4 v_Color;

uniform sampler2D u_Texture;

out vec4 o_Color;

void main() {
    vec4 texColor = texture(u_Texture, v_TexCoord);
    o_Color = texColor * v_Color;
}
