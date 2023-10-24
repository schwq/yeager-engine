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

class Yeager::Shader;

// Assimp model loading
yg_uint LoadTextureFromFile(yg_string path, bool flip = true);

// Imgui Icon
bool LoadTextureFromFile(yg_cchar filename, GLuint* out_texture, int* out_width, int* out_height);

namespace Yeager {
class Texture2D {
 public:
  Texture2D(yg_cchar texturePath, yg_string name = "DEFAULT");
  Texture2D(){};
  ~Texture2D();

  constexpr GLuint GetID() const { return m_id; }

  // Imported object use
  constexpr yg_string GetPath() { return m_texture_path; }
  constexpr void SetPath(yg_string path) { m_texture_path = path; }
  constexpr yg_string GetTypeName() { return m_type_name; }
  constexpr void SetTypeName(yg_string typeName) { m_type_name = typeName; }

 private:
  void GenerateTexture();
  void ReadDataToTexture(yg_cchar path);

  GLuint m_id = 0;
  yg_string m_name;
  yg_uint m_texture_num = 0;
  static yg_uint m_texture_count;
  // Imported object use
  yg_string m_texture_path;
  yg_string m_type_name;
};

class Skybox {
 public:
  Skybox(std::vector<yg_string> faces, yg_string name = "DEFAULT");
  ~Skybox();

  YEAGER_NODISCARD constexpr GLuint GetID() const { return m_id; }
  void Draw(Yeager::Shader* shader, yg_mat4 view, yg_mat4 projection);

 private:
  void GenerateTexture();
  void ReadDataToTexture(std::vector<yg_string> faces);

  GLuint m_id;
  yg_string m_name;
  yg_uint skyboxVAO, skyboxVBO;

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