#version 330 core
out vec4 fragColor;
in vec2 texCoords;
in vec3 NormalVec;
in vec3 FragPos;

#define MAX_LIGHTS 40

struct Material {
  sampler2D texture_diffuse1;
  sampler2D texture_specular1;
  float shininess;
};

struct PointLight {
  vec3 position;

  float constant;
  float linear;
  float quadratic;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;

  int m_active;  // 1 active
};

uniform PointLight lights[MAX_LIGHTS];
uniform Material material;
uniform vec3 viewPos;

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
  vec3 lightDir = normalize(-light.position - fragPos);
  float diff = max(dot(normal, lightDir), 0.0);
  vec3 reflectDir = reflect(-lightDir, normal);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
  float distance = length(light.position - fragPos);
  float attenuation = 1.0 / (light.constant + light.linear * distance +
                             light.quadratic * (distance * distance));
  vec3 ambient =
      light.ambient * vec3(texture(material.texture_diffuse1, texCoords));
  vec3 diffuse = light.diffuse * diff *
                 vec3(texture(material.texture_diffuse1, texCoords));
  vec3 specular = light.specular * spec *
                  vec3(texture(material.texture_specular1, texCoords));
  ambient *= attenuation;
  diffuse *= attenuation;
  specular *= attenuation;
  return (ambient + diffuse + specular);
}

void main() {
  vec3 norm = normalize(NormalVec);
  vec3 viewDir = normalize(viewPos - FragPos);
  vec3 result;
  for (int x = 0; x < MAX_LIGHTS; x++) {
    if (lights[x].m_active == 1) {
      result += CalcPointLight(lights[x], norm, FragPos, viewDir);
    }
  }

  fragColor = vec4(result, 1.0);
}
