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
#include "../Physics/PhysXHandle.h"
#include "Object.h"
#include "TextureHandle.h"

#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>

#define YEAGER_IMPORTER_DEFAULT_SOURCE "DEFAULT_SOURCE"

namespace Yeager {
class ApplicationCore;

#define YEAGER_ASSIMP_DEFAULT_FLAGS aiProcess_Triangulate | aiProcess_FlipUVs
#define YEAGER_ASSIMP_DEFAULT_FLAGS_ANIMATED \
  aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace | aiProcess_FlipUVs

enum ThreadType {
  ESingleThreaded,
  EMultiThreaded,
};

/// @brief this class can be used to import 2d and 3d models and assets to the editor by using assimp
class Importer {
 public:
  Importer(YgString source = YEAGER_IMPORTER_DEFAULT_SOURCE, ApplicationCore* app = YEAGER_NULLPTR);
  ~Importer();

  ObjectModelData Import(YgCchar path, bool flip_image = false,
                         unsigned int assimp_flags = YEAGER_ASSIMP_DEFAULT_FLAGS);
  AnimatedObjectModelData ImportAnimated(YgCchar path, bool flip_image = false,
                                         unsigned int assimp_flags = YEAGER_ASSIMP_DEFAULT_FLAGS_ANIMATED);
  PhysXTriangleMeshInput ImportToPhysX(YgCchar path, unsigned int assimp_flags = YEAGER_ASSIMP_DEFAULT_FLAGS);
  static unsigned int GetModelsCount() { return m_ImportedModelsCount; };

 protected:
  void ProcessNode(aiNode* node, const aiScene* scene, ObjectModelData* data);
  ObjectMeshData ProcessMesh(aiMesh* mesh, const aiScene* scene, ObjectModelData* data);
  std::vector<ObjectTexture*> LoadMaterialTexture(aiMaterial* material, aiTextureType type, YgString typeName,
                                                  CommonModelData* data);
  STBIDataOutput* LoadStbiDataOutput(YgString path, bool flip = false);

  void ProcessAnimatedNode(aiNode* node, const aiScene* scene, AnimatedObjectModelData* data);
  AnimatedObjectMeshData ProcessAnimatedMesh(aiMesh* mesh, const aiScene* scene, AnimatedObjectModelData* data);
  void SetVertexBoneDataToDefault(AnimatedVertexData& vertex);
  void SetVertexBoneData(AnimatedVertexData& vertex, int boneID, float weight);
  void ExtractBoneWeightForVertices(std::vector<AnimatedVertexData>& vertices, aiMesh* mesh, const aiScene* scene,
                                    AnimatedObjectModelData* data);

  void ProcessPhysXNode(aiNode* node, const aiScene* scene, PhysXTriangleMeshInput* input);
  void ProcessPhysXMesh(aiMesh* mesh, const aiScene* scene, PhysXTriangleMeshInput* input);

  static unsigned int m_ImportedModelsCount;
  ApplicationCore* m_Application = YEAGER_NULLPTR;
  YgString m_FullPath;
  YgString m_Source;
  bool m_ImageFlip = false;
};

class ImporterThreaded : public Importer {
 public:
  ImporterThreaded(YgString source, ApplicationCore* app);
  ~ImporterThreaded();
  virtual void ThreadImport(YgCchar path, bool flip_image = false,
                            unsigned int assimp_flags = YEAGER_ASSIMP_DEFAULT_FLAGS);
  bool IsThreadFinish();
  std::thread* GetThreadPtr() { return &m_Thread; }
  ObjectModelData GetValue() { return m_FutureObject.get(); }

 protected:
  std::promise<ObjectModelData> m_PromiseObject;
  std::future<ObjectModelData> m_FutureObject;
  std::atomic<bool> m_ThreadFinished = false;
  std::thread m_Thread;
  aiScene* m_Scene = YEAGER_NULLPTR;
  ObjectModelData m_Data;
};

class ImporterThreadedAnimated : public ImporterThreaded {
 public:
  ImporterThreadedAnimated(YgString source, ApplicationCore* app);
  ~ImporterThreadedAnimated();
  void ThreadImport(YgCchar path, bool flip_image = false, unsigned int assimp_flags = YEAGER_ASSIMP_DEFAULT_FLAGS);
  AnimatedObjectModelData GetValue() { return m_FutureObject.get(); }

 private:
  AnimatedObjectModelData m_Data;
  std::promise<AnimatedObjectModelData> m_PromiseObject;
  std::future<AnimatedObjectModelData> m_FutureObject;
};

}  // namespace Yeager