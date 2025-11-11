#version 330 core
out vec4 FragColor;

in vec3 v_worldNormal;
in vec3 v_worldPosition;

uniform vec3 u_holoColor;
uniform vec3 u_cameraPos;

void main()
{
    vec3 viewDir = normalize(u_cameraPos - v_worldPosition);
    
    // Efecto Fresnel/LayerWeight
    float facing = 1.0 - dot(v_worldNormal, viewDir);

    // Ajuste de curva (como el ColorRamp)
    float alpha = pow(facing, 3.0); // Juega con el '3.0' para cambiar el grosor

    FragColor = vec4(1.0, 0.0, 0.0, 1.0); // Forzar ROJO opaco
}