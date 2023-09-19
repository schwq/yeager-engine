#include "shader.h"

uint Shader::m_shader_count = 0;

Shader::Shader(const char* fragmentPath, const char* vertexPath, String name)
    : m_name(name) {
  m_shader_num = m_shader_count++;
  uint vt = CreateVertexGL(vertexPath);
  uint fg = CreateFragmentGL(fragmentPath);
  LinkShaders(vt, fg);
}

Shader::~Shader() {
  glDeleteProgram(m_id);
}

uint Shader::CreateVertexGL(const char* vertexPath) {
  uint vertexShaderSource = 0;
  int vertexShaderSuccess = 0;
  char vertexInfoLog[512];

  std::ifstream vertexFile =
      std::ifstream(vertexPath, std::ios_base::in | std::ios::ate);
  if (vertexFile.is_open()) {
    auto vertexFileSize = vertexFile.tellg();
    vertexFile.seekg(std::ios::beg);
    String vertexContent(vertexFileSize, 0);
    vertexFile.read(&vertexContent[0], vertexFileSize);

    const char* vertexReference = vertexContent.c_str();

    vertexShaderSource = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShaderSource, 1, &vertexReference, NULL);
    glCompileShader(vertexShaderSource);

    glGetShaderiv(vertexShaderSource, GL_COMPILE_STATUS, &vertexShaderSuccess);

    if (!vertexShaderSuccess) {
      glGetShaderInfoLog(vertexShaderSource, 512, NULL, vertexInfoLog);
      YeagerLog(ERROR, kSystemLog,
                       "Cannot create vertex shader: {}, ID: {}, Error: {}",
                       m_name.c_str(), m_shader_num, vertexInfoLog);
    } else {
      YeagerLog(INFO, kSystemLog, "Success in creating shader {}, ID {}",
                       m_name.c_str(), m_shader_num);
    }

    vertexFile.close();
  } else {
    YeagerLog(ERROR, kSystemLog, "Cannot open vertex shader file: {}",
                     vertexPath);
  }

  return vertexShaderSource;
}

uint Shader::CreateFragmentGL(const char* fragmentPath) {
  uint fragmentShaderSource = 0;
  int fragmentShaderSuccess = 0;
  char fragmentInfoLog[512];

  std::ifstream fragmentFile =
      std::ifstream(fragmentPath, std::ios_base::in | std::ios::ate);
  if (fragmentFile.is_open()) {
    auto fragmentFileSize = fragmentFile.tellg();
    fragmentFile.seekg(std::ios::beg);
    String fragmentContent(fragmentFileSize, 0);
    fragmentFile.read(&fragmentContent[0], fragmentFileSize);

    const char* fragmentReference = fragmentContent.c_str();

    fragmentShaderSource = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShaderSource, 1, &fragmentReference, NULL);
    glCompileShader(fragmentShaderSource);

    glGetShaderiv(fragmentShaderSource, GL_COMPILE_STATUS,
                  &fragmentShaderSuccess);

    if (!fragmentShaderSuccess) {
      glGetShaderInfoLog(fragmentShaderSource, 512, NULL, fragmentInfoLog);
      YeagerLog(ERROR, kSystemLog,
                       "Cannot create fragment shader: {}, ID: {}, Error: {}",
                       m_name.c_str(), m_shader_num, fragmentInfoLog);
    } else {
      YeagerLog(INFO, kSystemLog, "Success in creating shader {}, ID {}",
                       m_name.c_str(), m_shader_num);
    }

    fragmentFile.close();
  } else {
    YeagerLog(ERROR, kSystemLog, "Cannot open fragment shader file: {}",
                     fragmentPath);
  }

  return fragmentShaderSource;
}

void Shader::LinkShaders(uint vertexShader, uint fragmentShader) {
  m_id = glCreateProgram();
  glAttachShader(m_id, vertexShader);
  glAttachShader(m_id, fragmentShader);
  glLinkProgram(m_id);

  int linkSuccess;
  char linkInfo[512];

  glGetProgramiv(m_id, GL_LINK_STATUS, &linkSuccess);
  if (!linkSuccess) {
    glGetProgramInfoLog(m_id, 512, NULL, linkInfo);
    YeagerLog(ERROR, kSystemLog,
                     "Cannot link shaders: {}, ID: {}, Error: {}",
                     m_name.c_str(), m_shader_num, linkInfo);
  } else {
    YeagerLog(INFO, kSystemLog, "Success in linking shaders: {}, ID: {}",
                     m_name.c_str(), m_shader_num);
  }

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
}

void Shader::SetInt(const String& name, int value) {
  glUniform1i(glGetUniformLocation(m_id, name.c_str()), value);
}
void Shader::SetBool(const String& name, bool value) {
  glUniform1i(glGetUniformLocation(m_id, name.c_str()), (int)value);
}
void Shader::SetFloat(const String& name, float value) {
  glUniform1f(glGetUniformLocation(m_id, name.c_str()), value);
}
void Shader::SetMat4(const String& name, Matrix4 value) {
  glUniformMatrix4fv(glGetUniformLocation(m_id, name.c_str()), 1, GL_FALSE,
                     glm::value_ptr(value));
}
void Shader::SetVec3(const String& name, Vector3 value) {
  glUniform3fv(glGetUniformLocation(m_id, name.c_str()), 1, &value[0]);
}
void Shader::SetVec2(const String& name, glm::vec2 value) {
  glUniform2fv(glGetUniformLocation(m_id, name.c_str()), 1, &value[0]);
}
void Shader::SetUniform1i(const String& name, int value) {
  glUniform1i(glGetUniformLocation(m_id, name.c_str()), value);
}
void Shader::SetVec4(const String& name, glm::vec4 value) {
  glUniform4fv(glGetUniformLocation(m_id, name.c_str()), 1, &value[0]);
}

void Shader::UseShader() {
  glUseProgram(m_id);
}