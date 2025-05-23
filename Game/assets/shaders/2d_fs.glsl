#version 300 es
precision mediump float;

in vec2 v_TexCoord;
in vec4 v_Color;

uniform sampler2D u_Texture;
in vec4 v_pos;
out vec4 o_Color;

void main() {
    vec3 ndc = v_pos.xyz / v_pos.w;

    if (abs(ndc.x) > 1.0 || abs(ndc.y) > 1.0)
        discard;



    vec4 texColor = texture(u_Texture, v_TexCoord);
    o_Color = texColor * v_Color;

    if(o_Color.a < 0.1)
    {
        discard;
    }
}
