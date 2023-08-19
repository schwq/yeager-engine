#pragma once
#include "common.h"

namespace yaeger {

enum ErrorType {
  kGLFWError,
  kGLADError,
  kDirectXError,
  kVulkanError,
  kInterfaceError,
  kApplicationError,
  kTextureError,
  kCppError,
  kShaderError,
  kWindowError,
  kRendererError,
  kInputError,
  kCameraError,
  kError
};

struct Error {
  ErrorType m_type;
  String m_error;
  int m_code;
  void LogError();
};

}  // namespace yaeger