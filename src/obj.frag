#version 330 core
struct DirLight {
    vec3 direction;
    vec3 color;
};

struct PointLight {
    vec3 position;
    vec3 color;
    float linear;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
	
    vec3 color;
    
    float dropoff;
    float minDot;
};

in vec3 FragPos;
in vec3 Normal;
in vec4 normalColor;
in vec3 velocity;
in vec4 velocityColor;

out vec4 color;

uniform bool useNormal;
uniform bool showDirLight;
uniform bool showPointLight;
uniform bool showSpotLight;

uniform vec3 viewPos;
uniform vec3 Albedo;
uniform vec3 AmbientReflect;
uniform vec3 SpecularReflect;
uniform float PhongExp;
uniform DirLight dirLight;
uniform PointLight pointLight;
uniform SpotLight spotLight;

// Function prototypes
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{    
	if (useNormal) {
		color = velocityColor;
	} else {
		// Properties
		vec3 norm = normalize(Normal);
		vec3 viewDir = normalize(viewPos - FragPos);
		
		// == ======================================
		// Our lighting is set up in 3 phases: directional, point lights and an optional flashlight
		// For each phase, a calculate function is defined that calculates the corresponding color
		// per lamp. In the main() function we take all the calculated colors and sum them up for
		// this fragment's final color.
		// == ======================================
		// Phase 1: Directional lighting
		vec3 result = vec3(0, 0, 0);
		if (showDirLight) {
			result += CalcDirLight(dirLight, norm, viewDir);
		}
		//vec3 result = dirLight.color;
		// Phase 2: Point lights
		if (showPointLight) {
			result += CalcPointLight(pointLight, norm, FragPos, viewDir);    
		}
		// Phase 3: Spot light
		if (showSpotLight) {
			result += CalcSpotLight(spotLight, norm, FragPos, viewDir);    
		}
		
		color = vec4(result, 1.0);
	}
}

// Calculates the color when using a directional light.
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), PhongExp);
    // Combine results
    vec3 ambient = light.color * AmbientReflect;
    vec3 diffuse = light.color * diff * Albedo;
    vec3 specular = light.color * spec * SpecularReflect;
    return (ambient + diffuse + specular);
	//return diffuse;
}

// Calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), PhongExp);
    // Attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0f / (light.linear * distance); 
    //float attenuation = min(1, 1.0f / (light.linear * distance)); 
    // Combine results
    vec3 ambient = light.color * AmbientReflect;
    vec3 diffuse = light.color * diff * Albedo;
    vec3 specular = light.color * spec * SpecularReflect;
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

// Calculates the color when using a point light.
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), PhongExp);
    // Drop off
    float offset = -dot(lightDir, normalize(light.direction));
    float dropoff = 0;
	if (offset > light.minDot) {
		dropoff = pow(offset, light.dropoff);
	}
    // Combine results
    vec3 ambient = light.color * AmbientReflect;
    vec3 diffuse = light.color * diff * Albedo;
    vec3 specular = light.color * spec * SpecularReflect;
    float distance = length(light.position - fragPos);
    float attenuation = 1.0f / (0.1f * distance * distance);
    //float attenuation = min(1, 1.0f / (0.1f * distance * distance));
    ambient *= dropoff * attenuation;
    diffuse *= dropoff * attenuation;
    specular *= dropoff * attenuation;
    return (ambient + diffuse + specular);
}
