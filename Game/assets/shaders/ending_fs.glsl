#version 300 es
precision highp float;

in vec2 pos; // from vertex shader, range [-1, 1]

out vec4 FragColor;

uniform sampler2D floorTexture;
uniform sampler2D skyTexture;
uniform sampler2D catTexture;

uniform float width;
uniform float height;

uniform vec2  playerPos;
uniform float playerAngle;
uniform float cameraHeight;
uniform float fFoVHalf;  // half–field of view in radians

const vec3  fogColor   = vec3(0.7, 0.7, 0.8);
const float fogDensity = 0.5;

const vec2  baseCatSize = vec2(128.0, 256.0); // base cat size in pixels
const vec2  baseResolution = vec2(1280.0, 720.0);

// You can tweak this to shift cat position based on resolution
const vec2 catPositionOffsetAtMax = vec2(100.0, 50.0); // max offset at some resolution difference

void main() {
    vec2 fcScreen = (pos * 0.5 + 0.5) * vec2(width, height);

    float halfW = width  * 0.5;
    float halfH = height * 0.5;

    float dx = fcScreen.x - halfW;
    float dy = fcScreen.y - halfH;

    // floor/sky code omitted for brevity (same as your code)

    // forward/right basis
    vec2 forward = vec2(cos(playerAngle), sin(playerAngle));
    vec2 rightV  = vec2(-forward.y, forward.x);

    // intersection distance
    float rowDistance = cameraHeight / abs(dy) * halfH;

    vec2 worldPos = playerPos
                  + forward * rowDistance
                  + rightV  * (dx / halfW) * rowDistance;

    vec4 baseColor;
    if (fcScreen.y > halfH) {
        vec2 uv = sin(worldPos) * 0.5 + 0.5;
        uv = clamp(uv, 0.0, 1.0);
        uv = uv * 64.0;
        uv = floor(uv) / 64.0;
        baseColor = texture(skyTexture, uv);
    } else {
        vec2 uv = fract(worldPos);
        baseColor = texture(floorTexture, uv);
    }

    float fogFactor = exp(-fogDensity * rowDistance);
    fogFactor = clamp(fogFactor, 0.0, 1.0);

    vec3 fogged = mix(fogColor, baseColor.rgb, fogFactor);
    vec4 sceneColor = vec4(fogged, baseColor.a);

    // --- cat billboard overlay ---

    // Calculate resolution difference from base resolution
    vec2 resolutionDiff = vec2(width, height) - baseResolution;

    // Calculate cat size scaling or keep fixed:
    vec2 catSize = baseCatSize; // You can scale if you want

    // Calculate cat position offset based on resolution difference (clamp to avoid excessive offset)
    vec2 catOffset = clamp(resolutionDiff * 0.1, vec2(0.0), catPositionOffsetAtMax);

    // Screen center plus offset (e.g., move cat slightly right and up)
    vec2 screenCenter = vec2(halfW, halfH) + catOffset;

    vec2 catMin = floor(screenCenter - 0.5 * catSize);
    vec2 catMax = catMin + catSize;

    if (fcScreen.x >= catMin.x && fcScreen.x < catMax.x &&
        fcScreen.y >= catMin.y && fcScreen.y < catMax.y) {

        vec2 catUV = clamp((fcScreen - catMin) / catSize, 0.0, 1.0);
        catUV.y = 1.0 - catUV.y; // flip Y

        vec4 catColor = texture(catTexture, catUV);

        sceneColor = mix(sceneColor, catColor, catColor.a);
    }

    FragColor = sceneColor;
}
