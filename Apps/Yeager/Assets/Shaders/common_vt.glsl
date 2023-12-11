#version 330 core
#extension GL_ARB_separate_shader_objects : enable
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
out vec3 FragPos;
out vec2 texCoords;
out vec3 NormalVec;
uniform mat4 view;
uniform mat4 model;
uniform mat4 projection;

void main() {
  gl_Position = projection * view * model * vec4(aPos, 1.0f);
  texCoords = vec2(aTexCoords.x, aTexCoords.y);
  NormalVec = mat3(transpose(inverse(model))) * aNormal;
  FragPos = vec3(model * vec4(aPos, 1.0));
}