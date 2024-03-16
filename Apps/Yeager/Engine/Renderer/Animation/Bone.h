//    Yeager Engine, free and open source 3D/2D renderer written in OpenGL
//    In case of questions and bugs, please, refer to the issue tab on github
//    Repo : https://github.com/schwq/YeagerEngine
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

#include "../../../Common/Common.h"
#include "../../../Common/LogEngine.h"
#include "../../../Common/Utilities.h"

#include <assimp/anim.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <assimp/matrix4x4.h>
#include <assimp/vector3.h>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Yeager {
struct KeyPosition {
  Vector3 Position;
  float TimeStamp;
};

struct KeyRotation {
  glm::quat Orientation;
  float TimeStamp;
};

struct KeyScale {
  Vector3 Scale;
  float TimeStamp;
};

class Bone {
 public:
  Bone(const String& Name, int ID, const aiNodeAnim* Channel);

  void Update(float AnimationTime);

  constexpr Matrix4 GetLocalTransform() { return m_LocalTransform; }
  String GetBoneName() { return m_Name; }
  constexpr int GetBoneID() { return m_ID; }

  int GetPositionIndex(float AnimationTime);
  int GetRotationIndex(float AnimationTime);
  int GetScaleIndex(float AnimationTime);

 private:
  float GetScaleFactor(float LastTimeStamp, float NextTimeStamp, float AnimationTime);
  Matrix4 InterpolatePosition(float AnimationTime);
  Matrix4 InterpolateRotation(float AnimationTime);
  Matrix4 InterpolateScaling(float AnimationTime);

  std::vector<KeyPosition> m_Positions;
  std::vector<KeyRotation> m_Rotations;
  std::vector<KeyScale> m_Scales;
  int m_NumPositions;
  int m_NumRotations;
  int m_NumScalings;

  Matrix4 m_LocalTransform;
  String m_Name;
  int m_ID;
};
}  // namespace Yeager