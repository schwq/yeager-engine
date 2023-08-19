#pragma once

#include "../../common/common.h"
#include "../../application.h"
#include "shader.h"

class Shader;

bool LoadTextureFromFile(const char *filename, GLuint *out_texture,
                         int *out_width, int *out_height);

class EngineTexture2D {
 public:
  EngineTexture2D(const char *texturePath, String name = "DEFAULT");
  ~EngineTexture2D();

  GLuint GetID() { return m_id; }

 private:
  void GenerateTexture();
  void ReadDataToTexture(const char *path);

  GLuint m_id;
  String m_name;
  uint m_texture_num;
  static uint m_texture_count;
};

class EngineSkybox {
 public:
  EngineSkybox(std::vector<String> faces, String name = "DEFAULT");
  ~EngineSkybox();

  GLuint GetID() { return m_id; }
  void Draw(Shader *shader, Matrix4 view, Matrix4 projection);

 private:
  void GenerateTexture();
  void ReadDataToTexture(std::vector<String> faces);

  GLuint m_id;
  String m_name;
  unsigned int skyboxVAO, skyboxVBO;

  float skyboxVertices[108] = {
      // positions
      -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f,
      1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f,

      -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f,
      -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,

      1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,
      1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f,

      -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
      1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,

      -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,
      1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f,

      -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f,
      1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f};
};