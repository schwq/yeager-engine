#include "ShaderHandle.h"
using namespace Yeager;

Shader::Shader(Cchar fragmentPath, Cchar vertexPath, String name)
{
  m_Name = name;
  Uint vt = CreateVertexGL(vertexPath);
  Uint fg = CreateFragmentGL(fragmentPath);
  if (m_fragment_build && m_vertex_build) {
    LinkShaders(vt, fg);
    m_initialize = true;
  } else {
    Yeager::Log(-2, "Cannot link shaders! One of them have not initialized!");
  }
}

Shader::~Shader()
{
  // TODO here causing seg fault
  //glDeleteProgram(m_id);
}

Uint Shader::CreateVertexGL(Cchar vertexPath)
{
  Uint vertexShaderSource = 0;
  int vertexShaderSuccess = 0;
  char vertexInfoLog[512];

  std::ifstream vertexFile = std::ifstream(vertexPath, std::ios_base::in | std::ios::ate);
  if (vertexFile.is_open()) {
    auto vertexFileSize = vertexFile.tellg();
    vertexFile.seekg(std::ios::beg);
    String vertexContent(vertexFileSize, 0);
    vertexFile.read(&vertexContent[0], vertexFileSize);

    Cchar vertexReference = vertexContent.c_str();

    vertexShaderSource = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShaderSource, 1, &vertexReference, NULL);
    glCompileShader(vertexShaderSource);

    glGetShaderiv(vertexShaderSource, GL_COMPILE_STATUS, &vertexShaderSuccess);

    if (!vertexShaderSuccess) {
      glGetShaderInfoLog(vertexShaderSource, 512, NULL, vertexInfoLog);
      Yeager::Log(ERROR, "Cannot create vertex shader: {}, ID: {}, Error: {}", m_Name.c_str(), m_shader_num,
                  vertexInfoLog);
    } else {
      Yeager::Log(INFO, "Success in creating vertex shader {}, UUID {}", m_Name.c_str(), m_shader_num);
      m_vertex_build = true;
    }

    vertexFile.close();
  } else {
    Yeager::Log(ERROR, "Cannot open vertex shader file: {}", vertexPath);
  }

  return vertexShaderSource;
}

Uint Shader::CreateFragmentGL(Cchar fragmentPath)
{
  Uint fragmentShaderSource = 0;
  int fragmentShaderSuccess = 0;
  char fragmentInfoLog[512];

  std::ifstream fragmentFile = std::ifstream(fragmentPath, std::ios_base::in | std::ios::ate);
  if (fragmentFile.is_open()) {
    auto fragmentFileSize = fragmentFile.tellg();
    fragmentFile.seekg(std::ios::beg);
    String fragmentContent(fragmentFileSize, 0);
    fragmentFile.read(&fragmentContent[0], fragmentFileSize);

    Cchar fragmentReference = fragmentContent.c_str();

    fragmentShaderSource = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShaderSource, 1, &fragmentReference, NULL);
    glCompileShader(fragmentShaderSource);

    glGetShaderiv(fragmentShaderSource, GL_COMPILE_STATUS, &fragmentShaderSuccess);

    if (!fragmentShaderSuccess) {
      glGetShaderInfoLog(fragmentShaderSource, 512, NULL, fragmentInfoLog);
      Yeager::Log(ERROR, "Cannot create fragment shader: {}, ID: {}, Error: {}", m_Name.c_str(), m_shader_num,
                  fragmentInfoLog);
    } else {
      Yeager::Log(INFO, "Success in creating fragment shader {}, UUID {}", m_Name.c_str(), m_shader_num);
      m_fragment_build = true;
    }

    fragmentFile.close();
  } else {
    Yeager::Log(ERROR, "Cannot open fragment shader file: {}", fragmentPath);
  }

  return fragmentShaderSource;
}

void Shader::LinkShaders(Uint vertexShader, Uint fragmentShader)
{
  m_ShaderID = glCreateProgram();
  glAttachShader(m_ShaderID, vertexShader);
  glAttachShader(m_ShaderID, fragmentShader);
  glLinkProgram(m_ShaderID);

  int linkSuccess;
  char linkInfo[512];

  glGetProgramiv(m_ShaderID, GL_LINK_STATUS, &linkSuccess);
  if (!linkSuccess) {
    glGetProgramInfoLog(m_ShaderID, 512, NULL, linkInfo);
    Yeager::Log(ERROR, "Cannot link shaders: {}, ID: {}, Error: {}", m_Name.c_str(), m_shader_num, linkInfo);
  } else {
    Yeager::Log(INFO, "Success in linking shaders: {}, ID: {}", m_Name.c_str(), m_shader_num);
  }

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
}

void Shader::SetInt(const String& name, int value)
{
  glUniform1i(glGetUniformLocation(m_ShaderID, name.c_str()), value);
}
void Shader::SetBool(const String& name, bool value)
{
  glUniform1i(glGetUniformLocation(m_ShaderID, name.c_str()), (int)value);
}
void Shader::SetFloat(const String& name, float value)
{
  glUniform1f(glGetUniformLocation(m_ShaderID, name.c_str()), value);
}
void Shader::SetMat4(const String& name, Matrix4 value)
{
  glUniformMatrix4fv(glGetUniformLocation(m_ShaderID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}
void Shader::SetVec3(const String& name, Vector3 value)
{
  glUniform3fv(glGetUniformLocation(m_ShaderID, name.c_str()), 1, &value[0]);
}
void Shader::SetVec2(const String& name, glm::vec2 value)
{
  glUniform2fv(glGetUniformLocation(m_ShaderID, name.c_str()), 1, &value[0]);
}
void Shader::SetUniform1i(const String& name, int value)
{
  glUniform1i(glGetUniformLocation(m_ShaderID, name.c_str()), value);
}
void Shader::SetVec4(const String& name, glm::vec4 value)
{
  glUniform4fv(glGetUniformLocation(m_ShaderID, name.c_str()), 1, &value[0]);
}

void Shader::UseShader()
{
  glUseProgram(m_ShaderID);
}