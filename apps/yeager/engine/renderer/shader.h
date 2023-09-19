#pragma once


#include "../../common/common.h"
#include "../../common/utils.h"

class Shader {
 public:
  Shader(const char* fragmentPath, const char* vertexPath, String name);
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

 private:
  GLuint m_id;
  String m_name;
  bool m_initialize = false;
  static uint m_shader_count;
  uint m_shader_num = 0;

  uint CreateVertexGL(const char* vertexPath);
  uint CreateFragmentGL(const char* fragmentPath);
  void LinkShaders(uint vertexShader, uint fragmentShader);
};