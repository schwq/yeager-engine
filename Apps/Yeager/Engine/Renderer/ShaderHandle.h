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
  Shader(yg_cchar fragmentPath, yg_cchar vertexPath, yg_string name);
  ~Shader();

  void UseShader();
  void SetInt(const yg_string& name, int value);
  void SetBool(const yg_string& name, bool value);
  void SetFloat(const yg_string& name, float value);
  void SetMat4(const yg_string& name, yg_mat4 value);
  void SetVec3(const yg_string& name, yg_vec3 value);
  void SetVec2(const yg_string& name, glm::vec2 value);
  void SetUniform1i(const yg_string& name, int value);
  void SetVec4(const yg_string& name, glm::vec4 value);

 private:
  GLuint m_id;
  yg_string m_name;
  bool m_initialize = false;
  bool m_fragment_build = false;
  bool m_vertex_build = false;
  static yg_uint m_shader_count;
  yg_uint m_shader_num = 0;

  YEAGER_NODISCARD yg_uint CreateVertexGL(yg_cchar vertexPath);
  YEAGER_NODISCARD yg_uint CreateFragmentGL(yg_cchar fragmentPath);
  void LinkShaders(yg_uint vertexShader, yg_uint fragmentShader);
};

struct ShaderFromYaml {
  std::shared_ptr<Yeager::Shader> m_shader = nullptr;
  yg_string m_varName;
};

extern std::vector<ShaderFromYaml> ygConfigShaders;
Yeager::Shader* ShaderFromVarName(yg_string var);
}  // namespace Yeager