#include "ShaderHandle.h"
using namespace Yeager;

Shader::Shader(Cchar fragmentPath, Cchar vertexPath, String name)
{
  mShaderName = name;
  Uint vt = CreateVertexGL(vertexPath);
  Uint fg = CreateFragmentGL(fragmentPath);
  if (bIsFragmentShBuild && bIsVertexShBuild) {
    LinkShaders(vt, fg);
    bInitialize = true;
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
      Yeager::Log(ERROR, "Cannot create vertex shader: {}, ID: {}, Error: {}", mShaderName.c_str(), mShaderN,
                  vertexInfoLog);
    } else {
      Yeager::Log(INFO, "Success in creating vertex shader {}, UUID {}", mShaderName.c_str(), mShaderN);
      bIsVertexShBuild = true;
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
      Yeager::Log(ERROR, "Cannot create fragment shader: {}, ID: {}, Error: {}", mShaderName.c_str(), mShaderN,
                  fragmentInfoLog);
    } else {
      Yeager::Log(INFO, "Success in creating fragment shader {}, UUID {}", mShaderName.c_str(), mShaderN);
      bIsFragmentShBuild = true;
    }

    fragmentFile.close();
  } else {
    Yeager::Log(ERROR, "Cannot open fragment shader file: {}", fragmentPath);
  }

  return fragmentShaderSource;
}

void Shader::LinkShaders(Uint vertexShader, Uint fragmentShader)
{
  mShaderID = glCreateProgram();
  glAttachShader(mShaderID, vertexShader);
  glAttachShader(mShaderID, fragmentShader);
  glLinkProgram(mShaderID);

  int linkSuccess;
  char linkInfo[512];

  glGetProgramiv(mShaderID, GL_LINK_STATUS, &linkSuccess);
  if (!linkSuccess) {
    glGetProgramInfoLog(mShaderID, 512, NULL, linkInfo);
    Yeager::Log(ERROR, "Cannot link shaders: {}, ID: {}, Error: {}", mShaderName.c_str(), mShaderN, linkInfo);
  } else {
    Yeager::Log(INFO, "Success in linking shaders: {}, ID: {}", mShaderName.c_str(), mShaderN);
  }

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
}

void Shader::SetInt(const String& name, int value)
{
  glUniform1i(glGetUniformLocation(mShaderID, name.c_str()), value);
}
void Shader::SetBool(const String& name, bool value)
{
  glUniform1i(glGetUniformLocation(mShaderID, name.c_str()), (int)value);
}
void Shader::SetFloat(const String& name, float value)
{
  glUniform1f(glGetUniformLocation(mShaderID, name.c_str()), value);
}
void Shader::SetMat4(const String& name, Matrix4 value)
{
  glUniformMatrix4fv(glGetUniformLocation(mShaderID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}
void Shader::SetVec3(const String& name, Vector3 value)
{
  glUniform3fv(glGetUniformLocation(mShaderID, name.c_str()), 1, &value[0]);
}
void Shader::SetVec2(const String& name, glm::vec2 value)
{
  glUniform2fv(glGetUniformLocation(mShaderID, name.c_str()), 1, &value[0]);
}
void Shader::SetUniform1i(const String& name, int value)
{
  glUniform1i(glGetUniformLocation(mShaderID, name.c_str()), value);
}
void Shader::SetVec4(const String& name, glm::vec4 value)
{
  glUniform4fv(glGetUniformLocation(mShaderID, name.c_str()), 1, &value[0]);
}

void Shader::UseShader()
{
  glUseProgram(mShaderID);
}