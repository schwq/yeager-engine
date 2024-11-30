#version 460
#extension GL_ARB_separate_shader_objects : enable
layout(location = 0) in vec3 Position;
layout(location = 1) in vec2 TexCoords;
layout(location = 2) in vec3 Normals;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;
out vec4 Color;
out vec3 WorldPos;
out vec2 Tex;
out vec3 outNormal;
uniform float MinHeight;
uniform float MaxHeight;

void main()
{
  gl_Position = projection * view * model * vec4(Position, 1.0);

  float DeltaHeight = MaxHeight - MinHeight;
  float HeightRatio = (Position.y - MinHeight) / DeltaHeight;
  float c = HeightRatio * 0.8 + 0.2;
  Color = vec4(c, c, c, 1.0);
  WorldPos = Position;
  Tex = TexCoords;
  outNormal = Normals;
}