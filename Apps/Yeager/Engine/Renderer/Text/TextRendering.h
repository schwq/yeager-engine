//    Yeager Engine, free and open source 3D/2D renderer written in OpenGL
//    In case of questions and bugs, please, refer to the issue tab on github
//    Repo : https://github.com/schwq/YeagerEngine
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

#include <freetype2/ft2build.h>
#include FT_FREETYPE_H
#include "../../../Common/Common.h"
#include "../../../Common/LogEngine.h"
#include "../../../Common/Utilities.h"
#include "../Entity.h"

namespace Yeager {

class ApplicationCore;
class Shader;

struct Character {
  Uint TextureID = -1;
  IVector2 Size = IVector2(0);
  IVector2 Bearing = IVector2(0);
  Uint Advance = 0;
};

struct Text2D {
  Text2D(const String& text, const Vector2& position, const Vector3& color, const float scale)
      : Text(text), Position(position), Color(color), Scale(scale)
  {}
  String Text = YEAGER_NULL_LITERAL;
  Vector2 Position = Vector2(0.0f);
  Vector3 Color = YEAGER_ZERO_VECTOR3;
  float Scale = 1.0;
};

class TextRenderer {
 public:
  TextRenderer(Yeager::ApplicationCore* application);

  void Initialize();
  void LoadFont(const String& path, Uint size);

  ~TextRenderer();

  void BuildBuffers();

  void RenderText(Yeager::Shader* shader, const String& text, float x, float y, float scale, const Vector3& color);
  void RenderText(Yeager::Shader* shader, Transformation& transformation, const String& text, float x, float y,
                  float scale, const Vector3& color);
  std::pair<Uint, Text2D> AddText(Text2D& text);

 private:
  std::map<char, Character> m_Characters;
  std::map<Uint, Text2D> m_Texts;
  Uint m_TextIndexes = 0;
  FT_Library m_FTLibrary;
  FT_Face m_FTFace;
  Yeager::ApplicationCore* m_Application = YEAGER_NULLPTR;
  GLuint m_Vao, m_Vbo;
};

}  // namespace Yeager
