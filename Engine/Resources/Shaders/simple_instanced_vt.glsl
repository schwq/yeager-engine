#version 460 core
#extension GL_ARB_separate_shader_objects : enable
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

out vec2 texCoords;
out vec3 NormalVec;
out vec3 FragPos;
uniform mat4 view;
uniform mat4 projection;

uniform mat4 matrices[10];

void main()
{
  gl_Position = projection * view * matrices[gl_InstanceID] * vec4(aPos, 1.0f);
  texCoords = vec2(aTexCoords.x, aTexCoords.y);
  NormalVec = aNormal;
  FragPos = vec3(matrices[gl_InstanceID] * vec4(aPos, 1.0));
}