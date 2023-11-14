//    Yeager Engine, free and open source 3D/2D renderer written in OpenGL
//    In case of questions and bugs, please, refer to the issue tab on github
//    Repo : https://github.com/schwq/yeager-engine
//    Copyright (C) 2023
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include "../../Common/Common.h"
#include "../../Common/LogEngine.h"
#include "ShaderHandle.h"

// Assimp model loading
unsigned int LoadTextureFromFile(YgString path, bool flip = true);

// Imgui Icon
bool LoadTextureFromFile(YgCchar filename, GLuint* out_texture, int* out_width, int* out_height);

namespace Yeager {

class Shader;

class Texture2D {
 public:
  Texture2D(YgCchar texturePath, YgString name = "DEFAULT");
  Texture2D(){};
  ~Texture2D();

  YEAGER_NODISCARD constexpr GLuint GetID() const { return m_OpenGL_ID; }

  // Imported object use
  constexpr YgString GetPath() { return m_TexturePath; }
  constexpr void SetPath(YgString path) { m_TexturePath = path; }
  constexpr YgString GetTypeName() { return m_TypeName; }
  constexpr void SetTypeName(YgString typeName) { m_TypeName = typeName; }

  constexpr int GetWidth() const { return TextureWidth; }
  constexpr int GetHeight() const { return TextureHeight; }

 private:
  void GenerateTexture();
  void ReadDataToTexture(YgCchar path);

  // Used in imgui displaying the texture as an image
  int TextureWidth = 0, TextureHeight = 0;

  GLuint m_OpenGL_ID = 0;
  YgString m_Name;
  unsigned int m_TextureNumber = 0;
  static unsigned int m_TextureEngineCount;
  // Imported object use
  YgString m_TexturePath;
  YgString m_TypeName;
};

class Skybox {
 public:
  Skybox(std::vector<YgString> faces, YgString name = "DEFAULT");
  ~Skybox();

  YEAGER_NODISCARD constexpr GLuint GetID() const { return m_id; }
  void Draw(Yeager::Shader* shader, YgMatrix4 view, YgMatrix4 projection);

 private:
  void GenerateTexture();
  void ReadDataToTexture(std::vector<YgString> faces);

  GLuint m_id;
  YgString m_name;
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
}  // namespace Yeager