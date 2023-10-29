#version 330 core
out vec4 fragColor;
in vec2 texCoords;
in vec3 NormalVec;
uniform sampler2D texture_diffuse1;

void main()
{
  fragColor = texture(texture_diffuse1, texCoords);
}
