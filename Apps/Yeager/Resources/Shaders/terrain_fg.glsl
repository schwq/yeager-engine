#version 330
out vec4 FragColor;

in vec4 Color;
in vec2 Tex;
in vec3 WorldPos;
in vec3 outNormal;

uniform sampler2D TerrainTexture0;
uniform sampler2D TerrainTexture1;
uniform sampler2D TerrainTexture2;
uniform sampler2D TerrainTexture3;

uniform float TextureHeight0 = 64.0f;
uniform float TextureHeight1 = 128.0f;
uniform float TextureHeight2 = 192.0f;
uniform float TextureHeight3 = 256.0f;

uniform vec3 LightDir;

vec4 CalcColor()
{
  vec4 TexColor;
  float Height = WorldPos.y;

  if (Height < TextureHeight0) {
    TexColor = texture(TerrainTexture0, Tex);
  } else if (Height < TextureHeight1) {
    vec4 Color0 = texture(TerrainTexture0, Tex);
    vec4 Color1 = texture(TerrainTexture1, Tex);
    float Delta = TextureHeight1 - TextureHeight0;
    float Factor = (Height - TextureHeight0) / Delta;
    TexColor = mix(Color0, Color1, Factor);
  } else if (Height < TextureHeight2) {
    vec4 Color0 = texture(TerrainTexture1, Tex);
    vec4 Color1 = texture(TerrainTexture2, Tex);
    float Delta = TextureHeight2 - TextureHeight1;
    float Factor = (Height - TextureHeight1) / Delta;
    TexColor = mix(Color0, Color1, Factor);
  } else if (Height < TextureHeight3) {
    vec4 Color0 = texture(TerrainTexture2, Tex);
    vec4 Color1 = texture(TerrainTexture3, Tex);
    float Delta = TextureHeight3 - TextureHeight2;
    float Factor = (Height - TextureHeight2) / Delta;
    TexColor = mix(Color0, Color1, Factor);
  } else {
    TexColor = texture(TerrainTexture3, Tex);
  }

  return TexColor;
}

void main()
{
  vec4 TexColor = CalcColor();
  vec3 ambient = 0.35 * vec3(TexColor.x, TexColor.y, TexColor.z);
  vec3 norm = normalize(outNormal);
  vec3 lightdir = normalize(-LightDir);
  float diff = max(dot(norm, lightdir), 0.0);
  vec3 diffuse = vec3(Color.xyz) * diff * vec3(TexColor.x, TexColor.y, TexColor.z);
  vec3 result = diffuse + ambient;
  FragColor = vec4(result, 1.0);
}