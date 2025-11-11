#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

uniform sampler2D texture_diffuse1;  // textura original (piel)
uniform sampler2D u_noiseTexture;    // textura de ruido procedural
uniform float u_time;

uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
    // === ANIMACIÓN DE TEXTURA ESTILO AGUA ===
    vec2 uv = TexCoords;

    // Generamos movimiento en dos direcciones de ondas superpuestas
    float wave1 = texture(u_noiseTexture, uv + vec2(u_time * 0.025, u_time * 0.015)).r;
    float wave2 = texture(u_noiseTexture, uv - vec2(u_time * 0.02, u_time * 0.018)).r;

    // Combinamos las ondas para suavizar el patrón
    float combined = (wave1 + wave2) * 0.5;

    // Aplicamos desplazamiento suave (intensidad de 0.02 = sutil)
    uv += (combined - 0.5) * 0.05;

    // Muestra la textura desplazada
    vec4 baseColor = texture(texture_diffuse1, uv);

    // === ILUMINACIÓN BÁSICA (mantiene apariencia del modelo) ===
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor * baseColor.rgb;

    vec3 ambient = 0.25 * baseColor.rgb;

    // Reflejo tenue tipo agua
    float spec = pow(max(dot(reflect(-lightDir, norm), normalize(viewPos - FragPos)), 0.0), 32.0);
    vec3 specular = 0.2 * spec * lightColor;

    vec3 finalColor = ambient + diffuse + specular;

    FragColor = vec4(finalColor, baseColor.a);
}
