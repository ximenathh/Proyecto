// --- shaders/fuente.fs ---
#version 330 core
out vec4 FragColor;

in vec2 v_uv;
in vec3 v_worldPos; // <--- ¡NUEVO! Recibe la posición del mundo

uniform float u_time;
uniform sampler2D u_noiseTexture; 
uniform vec3 u_waterColor;

void main()
{
    // --- 1. SOLUCIÓN A LOS UVs ROTOS ---
    // Generamos UVs procedurales usando la posición XZ del mundo.
    // Esto ignora los 'v_uv' rotos de la malla 5.
    // (Multiplicamos por 0.2 para escalar la textura)
    vec2 procedural_uv = v_worldPos.xz * 0.2;

    // --- 2. Animación (usa los UVs procedurales) ---
    vec2 uv_animado_y = vec2(procedural_uv.x, procedural_uv.y - u_time * 0.2);
    float vibracion = texture(u_noiseTexture, vec2(procedural_uv.y * 0.5, u_time * 0.2)).r;
    float uv_x_vibrado = procedural_uv.x + (vibracion - 0.5) * 0.1; 
    vec2 uv_final = vec2(uv_x_vibrado, uv_animado_y.y);
    float ruido = texture(u_noiseTexture, uv_final).r;
    
    // 3. Patrón de ondas
    float alfa_ondas = smoothstep(0.4, 0.6, ruido);

    // 4. ¡SOLUCIÓN DE TRANSPARENCIA!
    // Controla el alfa máximo (0.6 = 60% opaco máximo)
    float transparencia_maestra = 0.6; 
    float alfa_final = alfa_ondas * transparencia_maestra;

    FragColor = vec4(vec3(1.0), alfa_final);
}