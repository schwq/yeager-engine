#pragma once

#include "../../application.h"
#include "../../common/common.h"
#include "shader.h"

class Application;
class Shader;

// Assimp model loading
uint LoadTextureFromFile(String path);

// Imgui Icon
bool LoadTextureFromFile(const char* filename, GLuint* out_texture,
                         int* out_width, int* out_height);

class EngineTexture2D {
 public:
  EngineTexture2D(const char* texturePath, Application* app,
                  String name = "DEFAULT");
  EngineTexture2D(){};
  ~EngineTexture2D();

  constexpr GLuint GetID() const { return m_id; }

  // Imported object use
  constexpr String GetPath() { return m_texture_path; }
  constexpr void SetPath(String path) { m_texture_path = path; }
  constexpr String GetTypeName() { return m_type_name; }
  constexpr void SetTypeName(String typeName) { m_type_name = typeName; }

 private:
  void GenerateTexture();
  void ReadDataToTexture(const char* path);

  GLuint m_id = 0;
  String m_name;
  uint m_texture_num = 0;
  Application* m_app = nullptr;
  static uint m_texture_count;
  // Imported object use
  String m_texture_path;
  String m_type_name;
};

class EngineSkybox {
 public:
  EngineSkybox(std::vector<String> faces, Application* app,
               String name = "DEFAULT");
  ~EngineSkybox();

  constexpr GLuint GetID() const { return m_id; }
  void Draw(Shader* shader, Matrix4 view, Matrix4 projection);

 private:
  void GenerateTexture();
  void ReadDataToTexture(std::vector<String> faces);

  GLuint m_id;
  String m_name;
  Application* m_app = nullptr;
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