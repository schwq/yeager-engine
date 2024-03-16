#include "PhysXGeometryHandle.h"
#include "../../Application.h"
#include "../Renderer/Importer.h"
using namespace Yeager;
using namespace physx;

std::vector<physx::PxShape*> Yeager::ExtractShapesFromActor(physx::PxRigidActor* actor)
{
  std::vector<PxShape*> shapes(actor->getNbShapes());
  actor->getShapes(reinterpret_cast<physx::PxShape**>(&shapes[0]), actor->getNbShapes());
  return shapes;
}

static physx::PxRigidDynamic* Yeager::CreateDynamic(Yeager::PhysXHandle* handle, const physx::PxTransform& trans,
                                                    const physx::PxGeometry& geometry, physx::PxMaterial& material,
                                                    const physx::PxVec3& velocity, physx::PxReal density)
{
  PxRigidDynamic* dynamic = PxCreateDynamic(*handle->GetPxPhysics(), trans, geometry, material, density);
  dynamic->setLinearVelocity(velocity);
  handle->PushToScene(dynamic);
  return dynamic;
}

physx::PxRigidStatic* Yeager::CreateStatic(Yeager::PhysXHandle* handle, const physx::PxTransform& trans,
                                           const physx::PxGeometry& geometry, physx::PxMaterial& material,
                                           physx::PxShape* shape)
{
  PxRigidStatic* stat = PxCreateStatic(*handle->GetPxPhysics(), trans, geometry, material);
  if (shape != YEAGER_NULLPTR) {
    stat->attachShape(*shape);
    PX_RELEASE(shape);
  }
  handle->PushToScene(stat);
  return stat;
}

PhysXGeometryHandle::PhysXGeometryHandle(PhysXHandle* handle, ApplicationCore* app)
    : m_PhysXHandle(handle), m_Application(app)
{
  Yeager::Log(INFO, "PhysXGeometryHandle was created!");
  if (!m_PhysXHandle->IsInitialized()) {
    Yeager::Log(WARNING, "PhysXHandle had not been initialized yet! PhysXGeometryHandle might crash!");
  }
}

PxRigidActor* PhysXGeometryHandle::CreatePrimitiveSphere(physx::PxU32 rigidType, physx::PxMaterial& material,
                                                         const physx::PxTransform& initialPos,
                                                         const physx::PxU32 radius, const physx::PxVec3& velocity,
                                                         physx::PxReal density)
{
  if (rigidType == YEAGER_PHYSX_RIGID_STATIC_BODY) {
    physx::PxRigidStatic* sphere = CreateStatic(m_PhysXHandle, initialPos, physx::PxSphereGeometry(radius), material);
    return sphere;
  } else if (rigidType == YEAGER_PHYSX_RIGID_DYNAMIC_BODY) {
    physx::PxRigidDynamic* sphere = CreateDynamic(m_PhysXHandle, initialPos, physx::PxSphereGeometry(radius), material);
    return sphere;
  }

  Yeager::Log(ERROR, "Create Primitive Sphere PhysX, the rigid type flag havent been set!");
  return YEAGER_NULLPTR;
}
physx::PxRigidActor* PhysXGeometryHandle::CreatePrimitiveBox(physx::PxU32 rigidType, physx::PxMaterial& material,
                                                             const physx::PxTransform& initialPos,
                                                             const physx::PxVec3 extents, const physx::PxVec3& velocity,
                                                             physx::PxReal density)
{
  if (rigidType == YEAGER_PHYSX_RIGID_STATIC_BODY) {
    physx::PxRigidStatic* box = CreateStatic(m_PhysXHandle, initialPos, physx::PxBoxGeometry(extents), material);
    return box;
  } else if (rigidType == YEAGER_PHYSX_RIGID_DYNAMIC_BODY) {
    physx::PxRigidDynamic* box = CreateDynamic(m_PhysXHandle, initialPos, physx::PxBoxGeometry(extents), material);
    return box;
  }

  Yeager::Log(ERROR, "Create Primitive Box PhysX, the rigid type flag havent been set!");
  return YEAGER_NULLPTR;
}

physx::PxRigidActor* PhysXGeometryHandle::CreatePrimitiveCapsule(physx::PxU32 rigidType, physx::PxMaterial& material,
                                                                 const physx::PxTransform& initialPos,
                                                                 const physx::PxU32 radius,
                                                                 const physx::PxU32 half_height,
                                                                 const physx::PxVec3& velocity, physx::PxReal density)
{
  if (rigidType == YEAGER_PHYSX_RIGID_STATIC_BODY) {
    physx::PxRigidStatic* capsule =
        CreateStatic(m_PhysXHandle, initialPos, physx::PxCapsuleGeometry(radius, half_height), material);
    return capsule;
  } else if (rigidType == YEAGER_PHYSX_RIGID_DYNAMIC_BODY) {
    physx::PxRigidDynamic* capsule =
        CreateDynamic(m_PhysXHandle, initialPos, physx::PxCapsuleGeometry(radius, half_height), material);
    return capsule;
  }
  Yeager::Log(ERROR, "Create Primitive Capsule PhysX, the rigid type flag havent been set!");
  return YEAGER_NULLPTR;
}

/* A plane divices space into above and below, it may only be created for static actors */
physx::PxRigidActor* PhysXGeometryHandle::CreatePrimitivePlane(physx::PxMaterial& material, const physx::PxF32 nx,
                                                               const physx::PxF32 ny, const physx::PxF32 nz,
                                                               const physx::PxF32 distance)
{
  PxRigidStatic* plane = PxCreatePlane(*m_PhysXHandle->GetPxPhysics(), PxPlane(nx, ny, nz, distance), material);
  m_PhysXHandle->PushToScene(plane);
  return plane;
}

PxTriangleMesh* PhysXGeometryHandle::CreateTriangleMesh(physx::PxU32 pointCount, physx::PxU32 trianglesCount,
                                                        physx::PxU32 pointStride, physx::PxU32 triangleStride,
                                                        physx::PxVec3* vertices, physx::PxU32* indices,
                                                        physx::PxU32 yeagerPhysxFlags)
{
  PxTriangleMeshDesc meshDesc;
  meshDesc.points.count = pointCount;
  meshDesc.points.stride = pointStride;
  meshDesc.points.data = vertices;

  meshDesc.triangles.count = trianglesCount;
  meshDesc.triangles.stride = triangleStride;
  meshDesc.triangles.data = indices;

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

PxTriangleMesh* PhysXGeometryHandle::CreateTriangleMesh(const Yeager::PhysXTriangleMeshInput& mesh,
                                                        physx::PxU32 yeagerPhysxFlags)
{
  PxTriangleMeshDesc meshDesc;
  meshDesc.points.count = mesh.Vertices.size();
  meshDesc.points.stride = sizeof(PxVec3);
  meshDesc.points.data = &mesh.Vertices[0];

  meshDesc.triangles.count = mesh.Vertices.size() / 3;
  meshDesc.triangles.stride = 3 * sizeof(PxU32);
  meshDesc.triangles.data = &mesh.Indices[0];

  PxTolerancesScale scale;
  PxCookingParams params(scale);

#ifdef YEAGER_DEBUG
  bool res = PxValidateTriangleMesh(params, meshDesc);
  PX_ASSERT(res);
#endif

  if (!meshDesc.isValid()) {
    Yeager::Log(ERROR, "PhysX cannot create a triangle mesh, the MeshDesc is not valid!");
  }

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

void PhysXGeometryHandle::RenderShapeInformation(physx::PxRigidActor* actor)
{
  std::vector<PxShape*> shapes = ExtractShapesFromActor(actor);
  for (auto& shape : shapes) {
    RenderGeometryInformation(actor, shape);
  }
}

void PhysXGeometryHandle::RenderGeometryInformation(physx::PxRigidActor* actor, physx::PxShape* shape)
{
  const PxGeometryHolder geom(shape->getGeometry());
  switch (geom.getType()) {
    case PxGeometryType::eBOX:
      ImGui::Text("Box Geometry");
      ImGui::Text("Extents x: %u y: %u z: %u", geom.box().halfExtents.x, geom.box().halfExtents.y,
                  geom.box().halfExtents.z);
      break;
    case PxGeometryType::eCAPSULE:
      ImGui::Text("Capsule Geometry");
      ImGui::Text("Half Height %u", geom.capsule().halfHeight);
      ImGui::Text("Radius %u", geom.capsule().radius);
      break;
    case PxGeometryType::eSPHERE:
      ImGui::Text("Sphere Geometry");
      ImGui::Text("Radius %u", geom.sphere().radius);
      break;
    case PxGeometryType::ePLANE:
      ImGui::Text("Plane Geometry");
      break;
    case PxGeometryType::eINVALID:
      ImGui::Text("INVALID_GEOMETRY_TYPE");
      break;
  }
}
