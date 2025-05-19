#version 300 es
precision mediump float;

// Inputs from vertex shader
in vec3 color;
in vec2 texCoord;
in vec3 Normal;
in vec3 crntPos;

// Uniforms
uniform sampler2D tex0;
uniform vec4 lightColor;
uniform vec3 lightPos;
uniform vec3 camPos;

// Output color
out vec4 FragColor;

void main()
{
    // Ambient lighting
    float ambient = 0.20;

    // Diffuse lighting
    vec3 normal = normalize(Normal);
    vec3 lightDirection = normalize(lightPos - crntPos);
    float diffuse = max(dot(normal, lightDirection), 0.0);

    // Specular lighting
    float specularLight = 0.50;
    vec3 viewDirection = normalize(camPos - crntPos);
    vec3 reflectionDirection = reflect(-lightDirection, normal);
    float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0), 8.0);
    float specular = specAmount * specularLight;

    // Final color output
    // * (lightColor * (diffuse + ambient + specular))
    FragColor = texture(tex0, texCoord * vec2(4));

    if(FragColor.a < 0.1)
    {
        discard;
    }
}
