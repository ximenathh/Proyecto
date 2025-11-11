#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUV; // <-- ¡AQUÍ ESTÁ LA LÍNEA FALTANTE!

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

out vec3 v_worldNormal;
out vec3 v_worldPosition;

void main()
{
    v_worldPosition = vec3(u_model * vec4(aPos, 1.0));
    v_worldNormal = normalize(mat3(transpose(inverse(u_model))) * aNormal);
    gl_Position = u_projection * u_view * vec4(v_worldPosition, 1.0);
    
    // No usamos 'aUV' para nada, pero necesitamos declararlo
    // para que el shader se "enganche" (link) correctamente con el modelo.
}