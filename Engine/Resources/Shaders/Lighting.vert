#version 460 core
#extension GL_ARB_separate_shader_objects : enable
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

out vec3 Color;
out vec3 NormalVec;
uniform mat4 view;
uniform mat4 model;
uniform mat4 projection;
uniform vec3 aColor;

void main()
{
  gl_Position = projection * view * model * vec4(aPos, 1.0f);
  Color = aColor;
  NormalVec = aNormal;
}