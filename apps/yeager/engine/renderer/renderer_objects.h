#pragma once

#include "../../application.h"
#include "../../common/common.h"

struct ObjectSpaceProp {
  Vector3 m_rotation = Vector3(0.0f);
  Vector3 m_position = Vector3(0.0f);
  Vector3 m_scaling = Vector3(1.0f);
  Matrix4 m_model = Matrix4(1.0f);
};

class ObjectsRenderer {
 public:
  ObjectsRenderer(){};

 private:
};