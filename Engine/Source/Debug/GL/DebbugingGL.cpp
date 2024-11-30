#include "DebbugingGL.h"
using namespace Yeager;

void GLAPIENTRY Yeager::GLMessageDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei lenght,
                                               const GLchar* message, const void* userParam)
{
  String src;
  switch (source) {
    case GL_DEBUG_SOURCE_API:
      src = "Source: API";
      break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
      src = "Source: Window System";
      break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
      src = "Source: Shader Compiler";
      break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:
      src = "Source: Third Party";
      break;
    case GL_DEBUG_SOURCE_APPLICATION:
      src = "Source: Application";
      break;
    case GL_DEBUG_SOURCE_OTHER:
      src = "Source: Other";
      break;
  }

  String t;
  switch (type) {
    case GL_DEBUG_TYPE_ERROR:
      t = "Type: Error";
      break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
      t = "Type: Deprecated Behaviour";
      break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
      t = "Type: Undefined Behaviour";
      break;
    case GL_DEBUG_TYPE_PORTABILITY:
      t = "Type: Portability";
      break;
    case GL_DEBUG_TYPE_PERFORMANCE:
      t = "Type: Performance";
      break;
    case GL_DEBUG_TYPE_MARKER:
      t = "Type: Marker";
      break;
    case GL_DEBUG_TYPE_PUSH_GROUP:
      t = "Type: Push Group";
      break;
    case GL_DEBUG_TYPE_POP_GROUP:
      t = "Type: Pop Group";
      break;
    case GL_DEBUG_TYPE_OTHER:
      t = "Type: Other";
      break;
  }

  String sev;
  switch (severity) {
    case GL_DEBUG_SEVERITY_HIGH:
      sev = "Severity: high";
      break;
    case GL_DEBUG_SEVERITY_MEDIUM:
      sev = "Severity: medium";
      break;
    case GL_DEBUG_SEVERITY_LOW:
      sev = "Severity: low";
      break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
      sev = "Severity: notification";
      break;
  }

  Yeager::LogDebug(ERROR, "GL CALLBACK: {}, Type {}, Severity {}, Message {}, Source {}",
                   type == GL_DEBUG_TYPE_ERROR ? "GL_ERROR" : "", t, sev, message, src);
}