#version 330 core
#extension GL_ARB_separate_shader_objects : enable
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitangent;
layout(location = 5) in ivec4 boneID;
layout(location = 6) in vec4 weight;

const int MAX_BONES = 300;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBonesMatrices[MAX_BONES];

out vec2 texCoords;
out vec3 NormalVec;
uniform mat4 view;
uniform mat4 model;
uniform mat4 projection;

void main()
{
  vec4 totalPosition = vec4(0.0f);
  for (int x = 0; x < MAX_BONE_INFLUENCE; x++) {

    if (boneID[x] == -1) {
      continue;
    }

    if (boneID[x] >= MAX_BONES) {
      totalPosition = vec4(aPos, 1.0f);
      break;
    }

    vec4 localPosition = finalBonesMatrices[boneID[x]] * vec4(aPos, 1.0f);
    totalPosition += localPosition * weight[x];
  }
  mat4 viewModel = view * model;
  gl_Position = projection * viewModel * totalPosition;
  texCoords = vec2(aTexCoords.x, aTexCoords.y);
  NormalVec = aNormal;
}