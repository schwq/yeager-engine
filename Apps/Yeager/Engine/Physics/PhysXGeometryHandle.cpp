#include "PhysXGeometryHandle.h"
#include "../../Application.h"
using namespace Yeager;
using namespace physx;

PhysXGeometryHandle::PhysXGeometryHandle(PhysXHandle* handle, ApplicationCore* app)
    : m_PhysXHandle(handle), m_Application(app)
{
  Yeager::Log(INFO, "PhysXGeometryHandle was created!");
  if (!m_PhysXHandle->IsInitialized()) {
    Yeager::Log(WARNING, "PhysXHandle had not been initialized yet! PhysXGeometryHandle might crash!");
  }
}

void PhysXGeometryHandle::CreateCapsule()
{
  PxRigidDynamic* aCapsuleActor = m_PhysXHandle->GetPxPhysics()->createRigidDynamic(PxTransform(PxVec3(0)));
  PxTransform relativePos(PxQuat(PxHalfPi, PxVec3(0, 0, 1)));
  PxMaterial* aMaterial = m_PhysXHandle->GetPxPhysics()->createMaterial(0, 0, 0);
  PxMaterial* materials[] = {aMaterial, aMaterial};
  PxShape* aCapsuleShape =
      PxRigidActorExt::createExclusiveShape(*aCapsuleActor, PxCapsuleGeometry(10, 4), materials, 2);
  aCapsuleShape->setLocalPose(relativePos);
  PxRigidBodyExt::updateMassAndInertia(*aCapsuleActor, 1);
  m_PhysXHandle->GetPxScene()->addActor(*aCapsuleActor);
}

bool PhysXGeometryHandle::CreateConvexMesh(YEAGER_UINT count, const std::vector<physx::PxVec3>& data,
                                           YEAGER_UINT stride, physx::PxConvexFlags flags)
{
  PxConvexMeshDesc convexDesc;
  convexDesc.points.count = count;
  convexDesc.points.stride = stride;
  convexDesc.points.data = data.data();
  convexDesc.flags = flags;

  PxTolerancesScale scale;
  PxCookingParams params(scale);

  PxDefaultMemoryOutputStream buffer;
  PxConvexMeshCookingResult::Enum result;

#ifdef YEAGER_DEBUG
  bool res = PxValidateConvexMesh(params, convexDesc);
  PX_ASSERT(res);
#endif

  if (!PxCookConvexMesh(params, convexDesc, buffer, &result)) {
    Yeager::Log(ERROR, "Cannot PxCookConvexMesh failed!");
    return false;
  }

  PxDefaultMemoryInputData input(buffer.getData(), buffer.getSize());
  PxConvexMesh* convexMesh = m_PhysXHandle->GetPxPhysics()->createConvexMesh(input);
  PxRigidDynamic* aConvexActor = m_PhysXHandle->GetPxPhysics()->createRigidDynamic(PxTransform(PxVec3(0)));
  PxMaterial* aMaterial = m_PhysXHandle->GetPxPhysics()->createMaterial(0, 0, 0);
  PxMaterial* materials[] = {aMaterial, aMaterial};
  PxShape* aConvexShape =
      PxRigidActorExt::createExclusiveShape(*aConvexActor, PxConvexMeshGeometry(convexMesh), materials, 2);
  PxRigidBodyExt::updateMassAndInertia(*aConvexActor, 1);
  m_PhysXHandle->GetPxScene()->addActor(*aConvexActor);
  return true;
}

PxTriangleMesh* PhysXGeometryHandle::CreateTriangleMesh(physx::PxU32 pointCount, physx::PxU32 trianglesCount,
                                                        physx::PxU32 pointStride, physx::PxU32 triangleStride,
                                                        const std::vector<PxVec3>& vertices,
                                                        const std::vector<physx::PxU32>& indices,
                                                        physx::PxTriangleMeshFlags flags, physx::PxU32 yeagerPhysxFlags)
{
  PxTriangleMeshDesc meshDesc;
  meshDesc.points.count = pointCount;
  meshDesc.points.stride = pointStride;
  meshDesc.points.data = vertices.data();

  meshDesc.triangles.count = trianglesCount;
  meshDesc.triangles.stride = triangleStride;
  meshDesc.triangles.data = indices.data();

  PxTolerancesScale scale;
  PxCookingParams params(scale);

  if (yeagerPhysxFlags == YEAGER_PHYSX_COOKING_STREAM_SERIALIZATION_DISABLED) {
    params.meshPreprocessParams |= PxMeshPreprocessingFlag::eDISABLE_CLEAN_MESH;
    params.meshPreprocessParams |= PxMeshPreprocessingFlag::eDISABLE_ACTIVE_EDGES_PRECOMPUTE;
  }

#ifdef YEAGER_DEBUG
  bool res = PxValidateTriangleMesh(params, meshDesc);
  PX_ASSERT(res);
#endif

  if (yeagerPhysxFlags == YEAGER_PHYSX_COOKING_STREAM_SERIALIZATION_ENABLED) {
    PxDefaultMemoryOutputStream buffer;
    PxTriangleMeshCookingResult::Enum result;
    if (!PxCookTriangleMesh(params, meshDesc, buffer, &result)) {
      Yeager::Log(ERROR, "Cannot PxCookConvexMesh failed!");
    }
    PxDefaultMemoryInputData input(buffer.getData(), buffer.getSize());
    return m_PhysXHandle->GetPxPhysics()->createTriangleMesh(input);
  } else {
    return PxCreateTriangleMesh(params, meshDesc, m_PhysXHandle->GetPxPhysics()->getPhysicsInsertionCallback());
  }
}
