//   Yeager Engine, free and open source 3D/2D renderer written in OpenGL
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

#include "PhysxAllocator.h"

namespace Yeager {
class ApplicationCore;
class PhysXHandle;

#define YEAGER_PHYSX_COOKING_STREAM_SERIALIZATION_ENABLED 0x01
#define YEAGER_PHYSX_COOKING_STREAM_SERIALIZATION_DISABLED 0x02
#define YEAGER_PHYSX_RIGID_DYNAMIC_BODY 0x03
#define YEAGER_PHYSX_RIGID_STATIC_BODY 0x04

static YEAGER_NODISCARD std::vector<physx::PxShape*> ExtractShapesFromActor(physx::PxRigidActor* actor);

struct PhysXTriangleMeshInput {
  std::vector<physx::PxVec3> Vertices;
  std::vector<physx::PxU32> Indices;
};

struct PhysXCapsule {
  physx::PxRigidDynamic* aCapsuleActor = YEAGER_NULLPTR;
  physx::PxTransform aRelativePose;
  physx::PxMaterial* aMaterial = YEAGER_NULLPTR;
  physx::PxShape* aCapsuleShape = YEAGER_NULLPTR;
};
struct PhysXTriangleMesh {
  physx::PxRigidStatic* aTriangleActor = YEAGER_NULLPTR;
  physx::PxTransform aRelativePose;
  physx::PxMaterial* aMaterial = YEAGER_NULLPTR;
  physx::PxShape* aTrianguleShape = YEAGER_NULLPTR;
  physx::PxTriangleMesh* aMesh = YEAGER_NULLPTR;
  physx::PxTriangleMeshGeometry* aGeometry = YEAGER_NULLPTR;
};

static physx::PxRigidDynamic* CreateDynamic(Yeager::PhysXHandle* handle, const physx::PxTransform& trans,
                                            const physx::PxGeometry& geometry, physx::PxMaterial& material,
                                            const physx::PxVec3& velocity = physx::PxVec3(0.0f),
                                            physx::PxReal density = 1.0f);

static physx::PxRigidStatic* CreateStatic(Yeager::PhysXHandle* handle, const physx::PxTransform& trans,
                                          const physx::PxGeometry& geometry, physx::PxMaterial& material,
                                          physx::PxShape* shape = YEAGER_NULLPTR);

class PhysXGeometryHandle {
 public:
  PhysXGeometryHandle(Yeager::PhysXHandle* handle, Yeager::ApplicationCore* app);
  ~PhysXGeometryHandle() {}

  bool CreateConvexMesh(physx::PxU32 count, const std::vector<physx::PxVec3>& data,
                        physx::PxU32 stride = sizeof(physx::PxVec3),
                        physx::PxConvexFlags flags = physx::PxConvexFlag::eCOMPUTE_CONVEX);
  YEAGER_NODISCARD physx::PxTriangleMesh* CreateTriangleMesh(
      physx::PxU32 pointCount, physx::PxU32 trianglesCount, physx::PxU32 pointStride, physx::PxU32 triangleStride,
      const std::vector<physx::PxVec3>& vertices, const std::vector<physx::PxU32>& indices,
      physx::PxTriangleMeshFlags flags,
      physx::PxU32 yeagerPhysxFlags = YEAGER_PHYSX_COOKING_STREAM_SERIALIZATION_ENABLED);

  YEAGER_NODISCARD physx::PxTriangleMesh* CreateTriangleMesh(
      const Yeager::PhysXTriangleMeshInput& mesh,
      physx::PxU32 yeagerPhysxFlags = YEAGER_PHYSX_COOKING_STREAM_SERIALIZATION_ENABLED);

  void CreateTriangleMeshModel(YgCchar path);

  /**
   * Primitives PhysX geometries types
   * Functions below create spheres, boxes, capsules and planes automatically and adds them to the scene
   * The user can specifiy if the primitive will be a rigid dynamic or static body by using the defined flags in this file
   */

  physx::PxRigidActor* CreatePrimitiveSphere(physx::PxU32 rigidType, physx::PxMaterial& material,
                                             const physx::PxTransform& initialPos, const physx::PxU32 radius,
                                             const physx::PxVec3& velocity = physx::PxVec3(0.0f),
                                             physx::PxReal density = 1.0f);

  void CreatePrimitiveBox(physx::PxU32 rigidType, physx::PxMaterial& material, const physx::PxTransform& initialPos,
                          const physx::PxVec3 extents, const physx::PxVec3& velocity = physx::PxVec3(0.0f),
                          physx::PxReal density = 1.0f);

  void CreatePrimitiveCapsule(physx::PxU32 rigidType, physx::PxMaterial& material, const physx::PxTransform& initialPos,
                              const physx::PxU32 radius, const physx::PxU32 half_height,
                              const physx::PxVec3& velocity = physx::PxVec3(0.0f), physx::PxReal density = 1.0f);

  /* A plane divices space into above and below, it may only be created for static actors */
  void CreatePrimitivePlane(physx::PxMaterial& material, const physx::PxF32 nx, const physx::PxF32 ny,
                            const physx::PxF32 nz, const physx::PxF32 distance);

  /**
   * Getters for render actors information, mostly used in the interface when displaying props from every actor in the scene.
   */

  void RenderShapeInformation(physx::PxRigidActor* actor);
  void RenderGeometryInformation(physx::PxRigidActor* actor, physx::PxShape* shape);

 private:
  Yeager::PhysXHandle* m_PhysXHandle = YEAGER_NULLPTR;
  Yeager::ApplicationCore* m_Application = YEAGER_NULLPTR;
};
}  // namespace Yeager
