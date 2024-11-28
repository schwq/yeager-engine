#version 460 core
out vec4 fragColor;
in vec2 texCoords;
in vec3 NormalVec;
in vec3 Color;

void main()
{
  fragColor = vec4(Color, 1.0);
}
