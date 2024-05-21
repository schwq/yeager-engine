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
#include "../Object.h"
#include "Bone.h"

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

struct AssimpNodeData {
  Matrix4 Transformation;
  String Name;
  int ChildrenCount;
  std::vector<AssimpNodeData> Children;
};

class Animation {
 public:
  Animation() = default;
  Animation(const String& name, const aiScene* scene, Uint index, AnimatedObject* model);
  ~Animation() {}

  Bone* FindBone(const String& name);

  inline float GetTicksPerSecond() { return m_TicksPerSecond; }
  inline float GetDuration() { return m_Duration; }
  inline const AssimpNodeData& GetRootNode() { return m_RootNode; }
  inline const std::map<String, BoneInfo>& GetBoneIDMap() { return m_BoneInfoMap; }

  String GetName() const { return m_Name; }
  Uint GetIndex() { return m_Index; }
  void SetIndex(Uint index) { m_Index = index; }

 private:
  void ReadMissingBones(const aiAnimation* animation, AnimatedObject& model);
  void ReadHeirarchyData(AssimpNodeData& dest, const aiNode* src);
  float m_Duration;
  int m_TicksPerSecond;
  std::vector<Bone> m_Bones;
  AssimpNodeData m_RootNode;
  String m_Name = YEAGER_NULL_LITERAL;
  std::map<String, BoneInfo> m_BoneInfoMap;
  Uint m_Index = 0;
};

}  // namespace Yeager