#version 460 core
#extension GL_ARB_separate_shader_objects : enable
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 Normals;
layout(location = 2) in vec2 texCoords;
out vec2 TexCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
  TexCoords = vec2(texCoords.x, texCoords.y);
  vec4 pos = projection * view * vec4(aPos, 1.0);
  gl_Position = pos.xyww;
}
