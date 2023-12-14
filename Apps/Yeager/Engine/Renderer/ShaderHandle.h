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
#include "../../Common/Utilities.h"

namespace Yeager {
class Shader {
 public:
  Shader(YgCchar fragmentPath, YgCchar vertexPath, YgString name);
  ~Shader();

  void UseShader();
  void SetInt(const YgString& name, int value);
  void SetBool(const YgString& name, bool value);
  void SetFloat(const YgString& name, float value);
  void SetMat4(const YgString& name, YgMatrix4 value);
  void SetVec3(const YgString& name, YgVector3 value);
  void SetVec2(const YgString& name, glm::vec2 value);
  void SetUniform1i(const YgString& name, int value);
  void SetVec4(const YgString& name, glm::vec4 value);

  constexpr inline GLuint GetId() { return m_id; }
  constexpr inline bool IsInitialized() { return m_initialize; }

   inline YgString GetName() { return m_Name; }

 private:
  GLuint m_id;
  bool m_initialize = false;
  bool m_fragment_build = false;
  bool m_vertex_build = false;
  static unsigned int m_shader_count;
  unsigned int m_shader_num = 0;

  YgString m_Name;

  YEAGER_NODISCARD unsigned int CreateVertexGL(YgCchar vertexPath);
  YEAGER_NODISCARD unsigned int CreateFragmentGL(YgCchar fragmentPath);
  void LinkShaders(unsigned int vertexShader, unsigned int fragmentShader);
};

struct ShaderFromYaml {
  std::shared_ptr<Yeager::Shader> m_shader = nullptr;
  YgString m_varName;
};

extern std::vector<ShaderFromYaml> ygConfigShaders;
Yeager::Shader* ShaderFromVarName(YgString var);
}  // namespace Yeager