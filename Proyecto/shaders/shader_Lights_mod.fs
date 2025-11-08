#version 330 core
out vec4 FragColor;

#define NUMBER 2
#define NUMBER_SPOT 1

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

struct DirLight
{
    vec3 direction;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight
{
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight
{
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
    
    float constant;
    float linear;
    float quadratic;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform vec3 viewPos;
uniform DirLight dirLight;
uniform PointLight pointLight[NUMBER];
uniform SpotLight spotLight[NUMBER_SPOT];

uniform sampler2D material_diffuse;
uniform sampler2D material_specular;
uniform float material_shininess;

// Function prototypes
vec3 CalcDirLight( DirLight light, vec3 normal, vec3 viewDir );
vec3 CalcPointLight( PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir );
vec3 CalcSpotLight( SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir );

void main()
{    
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 norm = normalize(Normal);

    // Luz direccional
    vec3 result = CalcDirLight(dirLight, norm, viewDir);
    
    // Luces puntuales
    for(int i = 0; i < NUMBER; i++)
    {
        result += CalcPointLight(pointLight[i], norm, FragPos, viewDir);
    }
    FragColor = vec4(result, 1.0);
}

// Calculates the color when using a directional light.
vec3 CalcDirLight( DirLight light, vec3 normal, vec3 viewDir )
{
    vec3 lightDir = normalize( -light.direction );
    
    // Diffuse shading
    float diff = max( dot( normal, lightDir ), 0.0 );
    
    // Specular shading
    vec3 reflectDir = reflect( -lightDir, normal );
    float spec = pow( max( dot( viewDir, reflectDir ), 0.0 ), material_shininess );
    
    // Combine results
    vec3 texColor = texture(material_diffuse, TexCoords).rgb;
    vec3 ambient = light.ambient * texColor;
    vec3 diffuse = light.diffuse * diff * texColor;
    
    // Specular usa la textura diffuse si no hay specular
    vec3 specularMap = texture(material_specular, TexCoords).rgb;
    if(length(specularMap) < 0.01)
        specularMap = vec3(0.5);
    vec3 specular = light.specular * spec * specularMap;
   
    return (ambient + diffuse + specular);
}

// Calculates the color when using a point light.
vec3 CalcPointLight( PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir )
{
    vec3 lightDir = normalize( light.position - fragPos );
    
    // Diffuse shading
    float diff = max( dot( normal, lightDir ), 0.0 );
    
    // Specular shading
    vec3 reflectDir = reflect( -lightDir, normal );
    float spec = pow( max( dot( viewDir, reflectDir ), 0.0 ), material_shininess );
    
    // Attenuation
    float distance = length( light.position - fragPos );
    float attenuation = 1.0f / ( light.constant + light.linear * distance + light.quadratic * ( distance * distance ) );
    
    // Combine results
    vec3 texColor = texture(material_diffuse, TexCoords).rgb;
    vec3 ambient = light.ambient * texColor;
    vec3 diffuse = light.diffuse * diff * texColor;
    
    // Specular usa la textura diffuse si no hay specular
    vec3 specularMap = texture(material_specular, TexCoords).rgb;
    if(length(specularMap) < 0.01)
        specularMap = vec3(0.5);
    vec3 specular = light.specular * spec * specularMap;
    
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}

// Calculates the color when using a spot light.
vec3 CalcSpotLight( SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir )
{
    vec3 lightDir = normalize( light.position - fragPos );
    
    // Diffuse shading
    float diff = max( dot( normal, lightDir ), 0.0 );
    
    // Specular shading
    vec3 reflectDir = reflect( -lightDir, normal );
    float spec = pow( max( dot( viewDir, reflectDir ), 0.0 ), material_shininess );
    
    // Attenuation
    float distance = length( light.position - fragPos );
    float attenuation = 1.0f / ( light.constant + light.linear * distance + light.quadratic * ( distance * distance ) );
    
    // Spotlight intensity
    float theta = dot( lightDir, normalize( -light.direction ) );
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp( ( theta - light.outerCutOff ) / epsilon, 0.0, 1.0 );
    
    // Combine results
    vec3 texColor = texture(material_diffuse, TexCoords).rgb;
    vec3 ambient = light.ambient * texColor;
    vec3 diffuse = light.diffuse * diff * texColor;
    
    // Specular usa la textura diffuse si no hay specular
    vec3 specularMap = texture(material_specular, TexCoords).rgb;
    if(length(specularMap) < 0.01)
        specularMap = vec3(0.5);
    vec3 specular = light.specular * spec * specularMap;
    
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    
    return ( ambient + diffuse + specular );
}