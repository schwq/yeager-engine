//    Yeager Engine, free and open source 3D/2D renderer written in OpenGL
//    In case of questions and bugs, please, refer to the issue tab on github
//    Repo : https://github.com/schwq/YeagerEngine
//    Copyright (C) 2023 - Present
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
#include "../../Common/Utilities.h"

namespace Yeager {
class Shader {
 public:
  Shader(Cchar fragmentPath, Cchar vertexPath, String name);
  ~Shader();

  void UseShader();
  void SetInt(const String& name, int value);
  void SetBool(const String& name, bool value);
  void SetFloat(const String& name, float value);
  void SetMat4(const String& name, Matrix4 value);
  void SetVec3(const String& name, Vector3 value);
  void SetVec2(const String& name, glm::vec2 value);
  void SetUniform1i(const String& name, int value);
  void SetVec4(const String& name, glm::vec4 value);

  constexpr inline GLuint GetId() { return m_ShaderID; }
  constexpr inline bool IsInitialized() { return m_initialize; }

  inline String GetName() { return m_Name; }
  inline String GetVarName() { return m_VarName; }
  inline void SetVarName(const String& str) { m_VarName = str; }

 private:
  GLuint m_ShaderID;
  bool m_initialize = false;
  bool m_fragment_build = false;
  bool m_vertex_build = false;
  static Uint m_shader_count;
  Uint m_shader_num = 0;

  String m_VarName;
  String m_Name;

  YEAGER_NODISCARD Uint CreateVertexGL(Cchar vertexPath);
  YEAGER_NODISCARD Uint CreateFragmentGL(Cchar fragmentPath);
  void LinkShaders(Uint vertexShader, Uint fragmentShader);
};
}  // namespace Yeager