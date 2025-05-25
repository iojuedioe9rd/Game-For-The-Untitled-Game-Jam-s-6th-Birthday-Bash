#version 300 es
precision mediump float;

out vec4 FragColor;
in vec2 texCoords;

uniform sampler2D screenTexture;

const float offset_x = 1.0 / 800.0;
const float offset_y = 1.0 / 800.0;

vec2 offsets[9] = vec2[](
    vec2(-offset_x,  offset_y), vec2( 0.0,  offset_y), vec2( offset_x,  offset_y),
    vec2(-offset_x,  0.0),      vec2( 0.0,  0.0),      vec2( offset_x,  0.0),
    vec2(-offset_x, -offset_y), vec2( 0.0, -offset_y), vec2( offset_x, -offset_y)
);

float kernel[9] = float[](
    0.0625, 0.125, 0.0625,
    0.125,  0.25,  0.125,
    0.0625, 0.125, 0.0625
);

vec3 resize(float Sample, vec3 colour)
{
    float r = colour.r * Sample;
    r = floor(r) / Sample;
    float g = colour.g * Sample;
    g = floor(g) / Sample;
    float b = colour.b * Sample;
    b = floor(b) / Sample;
    return vec3(r, g, b);
}

void main() {
    float PixelSample = 64.0;

    vec2 UV = texCoords * PixelSample;
    UV = floor(UV) / PixelSample;

    vec3 sampledColor = vec3(0.0);
    for (int i = 0; i < 9; i++) {
        sampledColor += texture(screenTexture, UV + offsets[i]).rgb * kernel[i];
    }

    float grey = dot(sampledColor, vec3(0.21, 0.71, 0.07));

    vec3 base = texture(screenTexture, texCoords).rgb;
    vec3 finalColor = base + grey * 0.05;

    // Vignette effect
    vec2 centered = UV - vec2(0.5);
    float dist = length(centered);
    float vignette = smoothstep(0.75, 0.35, dist); // fade from 0.45 to 0.75 radius
    finalColor *= vignette;

    FragColor = vec4(resize(1024.0, finalColor), 1.0);
}
