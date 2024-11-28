//    Yeager Engine, free and open source 3D/2D renderer written in OpenGL
//    In case of questions and bugs, please, refer to the issue tab on github
//    Repo : https://github.com/schwq/YeagerEngine
//    Copyright (C) 2023 - Present
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

#include "Common/Utils/Common.h"
#include "Common/Utils/LogEngine.h"
#include "Common/Utils/Utilities.h"
#include "Components/Physics/PhysXHandle.h"
#include "Components/Renderer/Objects/Object.h"
#include "Components/Renderer/Texture/TextureHandle.h"

#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>

#define YEAGER_IMPORTER_DEFAULT_SOURCE "DEFAULT_SOURCE"

namespace Yeager {
class ApplicationCore;
struct CustomTextureFolder;

#define YEAGER_ASSIMP_DEFAULT_FLAGS aiProcess_Triangulate
#define YEAGER_ASSIMP_DEFAULT_FLAGS_ANIMATED \
  aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace

enum ThreadType {
  ESingleThreaded,
  EMultiThreaded,
};

/**
    https://www.khronos.org/opengl/wiki/Calculating_a_Surface_Normal
    A surface normal for a triangle can be calculated by taking the vector cross product of two edges of 
    that triangle. The order of the vertices used in the calculation will affect the direction of the normal (in or out of the face w.r.t. winding).
    So for a triangle p1, p2, p3, if the vector U = p2 - p1 and the vector V = p3 - p1 then 
    the normal N = U X V and can be calculated by:
    Nx = UyVz - UzVy
    Ny = UzVx - UxVz
    Nz = UxVy - UyVx 
 */
extern Vector3 CalculateTriangleNormalFromPoints(Vector3 p1, Vector3 p2, Vector3 p3);
/* Newell's Method */
extern Vector3 CalculatePolygonNormalFromPoints(std::vector<Vector3>& points);

extern std::vector<physx::PxVec3> ConvertYgVectors3ToPhysXVec3(const std::vector<Vector3>& vectors);

/// @brief this class can be used to import 2d and 3d models and assets to the editor by using assimp
class Importer {
 public:
  Importer(String source = YEAGER_IMPORTER_DEFAULT_SOURCE, ApplicationCore* app = YEAGER_NULLPTR);
  ~Importer();

  ObjectModelData Import(Cchar path, const ObjectCreationConfiguration configuration = ObjectCreationConfiguration(),
                         bool flip_image = false, Uint assimp_flags = YEAGER_ASSIMP_DEFAULT_FLAGS);
  AnimatedObjectModelData ImportAnimated(
      Cchar path, const ObjectCreationConfiguration configuration = ObjectCreationConfiguration(),
      bool flip_image = false, Uint assimp_flags = YEAGER_ASSIMP_DEFAULT_FLAGS_ANIMATED);
  ObjectModelData ImportToPhysX(Cchar path, physx::PxRigidActor* actor, bool flip_image = false,
                                Uint assimp_flags = YEAGER_ASSIMP_DEFAULT_FLAGS);

  static Uint GetModelsCount() { return m_ImportedModelsCount; };

 protected:
  void ProcessNode(aiNode* node, const aiScene* scene, ObjectModelData* data);
  ObjectMeshData ProcessMesh(aiMesh* mesh, const aiScene* scene, ObjectModelData* data);
  std::vector<MaterialTexture2D*> LoadMaterialTexture(aiMaterial* material, aiTextureType type, String typeName,
                                                      CommonModelData* data);
  STBIDataOutput* LoadStbiDataOutput(String path, bool flip = false);

  void ProcessAnimatedNode(aiNode* node, const aiScene* scene, AnimatedObjectModelData* data);
  AnimatedObjectMeshData ProcessAnimatedMesh(aiMesh* mesh, const aiScene* scene, AnimatedObjectModelData* data);
  void SetVertexBoneDataToDefault(AnimatedVertexData& vertex);
  void SetVertexBoneData(AnimatedVertexData& vertex, int boneID, float weight);
  void ExtractBoneWeightForVertices(std::vector<AnimatedVertexData>& vertices, aiMesh* mesh, const aiScene* scene,
                                    AnimatedObjectModelData* data);

  void ProcessPhysXNode(physx::PxRigidActor* actor, aiNode* node, const aiScene* scene, ObjectModelData* data);
  ObjectMeshData ProcessPhysXMesh(physx::PxRigidActor* actor, aiMesh* mesh, const aiScene* scene,
                                  ObjectModelData* data);

  static Uint m_ImportedModelsCount;
  ApplicationCore* m_Application = YEAGER_NULLPTR;
  ObjectCreationConfiguration m_CreationConfiguration;
  String m_FullPath;
  String m_Source;
  bool m_ImageFlip = false;
};

class ImporterThreaded : public Importer {
 public:
  ImporterThreaded(String source, ApplicationCore* app);
  ~ImporterThreaded();
  virtual void ThreadImport(Cchar path, const ObjectCreationConfiguration configuration = ObjectCreationConfiguration(),
                            bool flip_image = false, Uint assimp_flags = YEAGER_ASSIMP_DEFAULT_FLAGS);
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
  ImporterThreadedAnimated(String source, ApplicationCore* app);
  ~ImporterThreadedAnimated();
  void ThreadImport(Cchar path, const ObjectCreationConfiguration configuration = ObjectCreationConfiguration(),
                    bool flip_image = false, Uint assimp_flags = YEAGER_ASSIMP_DEFAULT_FLAGS);
  AnimatedObjectModelData GetValue() { return m_FutureObject.get(); }

 private:
  AnimatedObjectModelData m_Data;
  std::promise<AnimatedObjectModelData> m_PromiseObject;
  std::future<AnimatedObjectModelData> m_FutureObject;
};

}  // namespace Yeager