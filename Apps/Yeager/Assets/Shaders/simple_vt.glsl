#version 330 core
#extension GL_ARB_separate_shader_objects : enable
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

out vec2 texCoords;
out vec3 NormalVec;
uniform mat4 view;
uniform mat4 model;
uniform mat4 projection;

uniform vec3 offsets[1000];

void main()
{
  gl_Position = projection * view * model * vec4(aPos + offsets[gl_InstanceID], 1.0f);
  texCoords = vec2(aTexCoords.x, aTexCoords.y);
  NormalVec = aNormal;
}