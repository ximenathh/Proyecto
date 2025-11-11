// --- shaders/fuente.vs ---
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUV;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 v_uv;
out vec3 v_worldPos; // <--- ¡NUEVO! Pasa la posición del mundo

void main()
{
    v_worldPos = vec3(model * vec4(aPos, 1.0)); // <--- ¡NUEVO!
    gl_Position = projection * view * vec4(v_worldPos, 1.0);
    v_uv = aUV;
}