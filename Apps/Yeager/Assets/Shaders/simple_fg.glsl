#version 330 core
out vec4 fragColor;
in vec2 texCoords;
in vec3 NormalVec;
in vec3 FragPos;


struct Material {
	float Shininess;
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
};

struct Viewer {
	vec3 Position;
};

struct DirectionalLight {
	vec3 Direction;
	vec3 Ambient;
	vec3 Diffuse;
	vec3 Specular;
	vec3 Color;
};

#define MAX_POINT_LIGHTS 10

struct PointLight {
	vec3 Position;
	float Constant;
	float Linear;
	float Quadratic;
	vec3 Ambient;
	vec3 Diffuse;
	vec3 Specular;
	bool Active;
};

struct SpotLight {
	vec3 Position;
	vec3 Direction;
	float CutOff;
	float OuterCutOff;
	float Constant;
	float Linear;
	float Quadratic;
	vec3 Ambient;
	vec3 Diffuse;
	vec3 Specular;
};

uniform SpotLight spotLight;
uniform DirectionalLight directionalLight;
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform Material material;
uniform Viewer viewer;


vec3 CalcDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
	vec3 norm = normalize(NormalVec);
	vec3 viewDir = normalize(viewer.Position - FragPos);

	vec3 result = CalcDirectionalLight(directionalLight, norm, viewDir);

	for(int x = 0; x < MAX_POINT_LIGHTS; x++) {
		if(pointLights[x].Active) {
			result += CalcPointLight(pointLights[x], norm, FragPos, viewDir);
		}
	}

	result += CalcSpotLight(spotLight, norm, FragPos, viewDir);
	fragColor = vec4(result, 1.0);
}

vec3 CalcDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir) {
	vec3 lightDir = normalize(-light.Direction);
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.Shininess);
	vec3 ambient = light.Ambient * vec3(texture(material.texture_diffuse1, texCoords));
	vec3 diffuse = light.Diffuse * diff * vec3(texture(material.texture_diffuse1, texCoords));
	vec3 specular = light.Specular * spec * vec3(texture(material.texture_specular1, texCoords));
	return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
	vec3 lightDir = normalize(light.Position - fragPos);
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.Shininess);
	float _distance = length(light.Position - fragPos);
	float attenuation = 1.0 / (light.Constant + light.Linear * _distance + light.Quadratic * (_distance * _distance));
	vec3 ambient = light.Ambient * vec3(texture(material.texture_diffuse1, texCoords));
	vec3 diffuse = light.Diffuse * diff * vec3(texture(material.texture_diffuse1, texCoords));
	vec3 specular = light.Specular * spec * vec3(texture(material.texture_specular1, texCoords));
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;
	return (ambient + diffuse + specular);
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
	vec3 lightDir = normalize(light.Position - fragPos);
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.Shininess);
	float _distance = length(light.Position - fragPos);
	float attenuantion  = 1.0 / (light.Constant + light.Linear * _distance + light.Quadratic * (_distance * _distance));
	float theta = dot(lightDir, normalize(-light.Direction));
	float epsilon = light.CutOff - light.OuterCutOff;
	float intesity = clamp((theta - light.OuterCutOff) / epsilon, 0.0, 1.0);

	vec3 ambient = light.Ambient * vec3(texture(material.texture_diffuse1, texCoords));
	vec3 diffuse = light.Diffuse * diff * vec3(texture(material.texture_diffuse1, texCoords));
	vec3 specular = light.Specular * spec * vec3(texture(material.texture_specular1, texCoords));
	ambient *= attenuantion * intesity;
	diffuse *= attenuantion * intesity;
	specular *= attenuantion * intesity;
	return (ambient + diffuse + specular);
}
