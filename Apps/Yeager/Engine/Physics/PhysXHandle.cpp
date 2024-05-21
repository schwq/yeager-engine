#include "PhysXHandle.h"
#include "../../Application.h"
#include "PhysXGeometryHandle.h"
using namespace Yeager;
using namespace physx;

extern physx::PxMat44 Yeager::Matrix4ToPxMat44(const Matrix4& mat)
{
  physx::PxMat44 m = physx::PxMat44(1.0f);
  m.column0 = physx::PxVec4(mat[0][0], mat[1][0], mat[2][0], mat[3][0]);
  m.column1 = physx::PxVec4(mat[0][1], mat[1][1], mat[2][1], mat[3][1]);
  m.column2 = physx::PxVec4(mat[0][2], mat[1][2], mat[2][2], mat[3][2]);
  m.column3 = physx::PxVec4(mat[0][3], mat[1][3], mat[2][3], mat[3][3]);
  return m;
}

extern Matrix4 Yeager::PxMat4ToMatrix4(const physx::PxMat44& mat)
{
  Matrix4 m = Matrix4(1.0f);
  m[0][0] = mat.column0.x;
  m[1][0] = mat.column0.y;
  m[2][0] = mat.column0.z;
  m[3][0] = mat.column0.w;
  m[0][1] = mat.column1.x;
  m[1][1] = mat.column1.y;
  m[2][1] = mat.column1.z;
  m[3][1] = mat.column1.w;
  m[0][2] = mat.column2.x;
  m[1][2] = mat.column2.y;
  m[2][2] = mat.column2.z;
  m[3][2] = mat.column2.w;
  m[0][3] = mat.column3.x;
  m[1][3] = mat.column3.y;
  m[2][3] = mat.column3.z;
  m[3][3] = mat.column3.w;
  return m;
}

PxVec3 Yeager::Vector3ToPxVec3(const Vector3& vec)
{
  PxVec3 rt;
  rt.x = vec.x;
  rt.y = vec.y;
  rt.z = vec.z;
  return rt;
}

Vector3 Yeager::PxVec3ToVector3(const PxVec3& vec)
{
  Vector3 rt(0.0f);
  rt.x = vec.x;
  rt.y = vec.y;
  rt.z = vec.z;
  return rt;
}

void PhysXHandle::PushToScene(physx::PxRigidActor* actor)
{
  m_PxScene->addActor(*actor);
  m_PxActors.push_back(actor);
}

PhysXHandle::PhysXHandle(Yeager::ApplicationCore* app) : m_Application(app) {}

bool PhysXHandle::InitPxEngine()
{
  Yeager::Log(INFO, "Initialize Nvidia PhysX engine! Version {}.{}", PX_PHYSICS_VERSION_MAJOR,
              PX_PHYSICS_VERSION_MINOR);

  m_PxFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_PxAllocatorCallback, m_PxErrorCallback);

  if (!m_PxFoundation) {
    Yeager::Log(ERROR, "PhysX cannot create the PxFoundation!");
    return false;
  }

  m_PxPvd = PxCreatePvd(*m_PxFoundation);

  if (!m_PxPvd) {
    Yeager::Log(ERROR, "PxCreatePvd failed! PhysX Visual Debugger cannot be created!");
    return false;
  }

  m_PxPvdTransport = PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
  if (!m_PxPvd->connect(*m_PxPvdTransport, PxPvdInstrumentationFlag::eALL)) {
    Yeager::Log(ERROR, "m_PxPvd cannot connect to PVD!");
  } else {
    Yeager::Log(INFO, "PhysX connect to PVD success! Host {} Port {}", PVD_HOST, 5425);
  }
  m_PxPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_PxFoundation, PxTolerancesScale(), true, m_PxPvd);

  if (!m_PxPhysics) {
    Yeager::Log(ERROR, "PhysX cannot create PxPhysics!");
    return false;
  }

  if (!PxInitExtensions(*m_PxPhysics, nullptr)) {
    Yeager::Log(ERROR, "PxInitExtensions failed!");
    m_PxExtensionsEnabled = false;
  }

  if (!m_PxPhysics->getTolerancesScale().isValid()) {
    Yeager::Log(WARNING, "PxPhysx tolerance scale is not valid!");
  }

  m_PxCpuDispatcher = PxDefaultCpuDispatcherCreate(Yeager::GetHardwareThreadCount() / 2);
  if (!m_PxCpuDispatcher) {
    Yeager::Log(ERROR, "PxDefaultCpuDispacterCreate failed!");
    return false;
  }

  m_PxSceneDesc = new PxSceneDesc(m_PxPhysics->getTolerancesScale());
  m_PxSceneDesc->gravity = PxVec3(0.0f, -90.81f, 0.0f);
  m_PxSceneDesc->cpuDispatcher = m_PxCpuDispatcher;
  m_PxSceneDesc->filterShader = PxDefaultSimulationFilterShader;

  if (!m_PxSceneDesc->isValid()) {
    Yeager::Log(WARNING, "PxSceneDesc is not valid!");
  }

  m_PxScene = m_PxPhysics->createScene(*m_PxSceneDesc);

  m_PxPvdClient = m_PxScene->getScenePvdClient();
  if (m_PxPvdClient) {
    m_PxPvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
    m_PxPvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
    m_PxPvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
  }

  PxMaterial* gMaterial = m_PxPhysics->createMaterial(0.5f, 0.5f, 0.6f);
  PxRigidStatic* groundPlane = PxCreatePlane(*m_PxPhysics, PxPlane(0, 1, 0, 0), *gMaterial);
  m_PxActors.push_back(groundPlane);
  m_PxScene->addActor(*groundPlane);

  m_PhysXGeometryHandle = new PhysXGeometryHandle(this, m_Application);
  m_CharacterController = new PhysXCharacterController(this);

  m_Initialized = true;
  return true;
}

void PhysXHandle::TerminateEngine()
{
  for (auto& capsule : m_Capsules) {
    delete capsule;
  }
  m_Capsules.clear();

  m_CharacterController->PhysXCharacterController::~PhysXCharacterController();

  PX_RELEASE(m_PxScene);
  PX_RELEASE(m_PxPhysics);
  if (m_PxExtensionsEnabled) {
    PxCloseExtensions();
  }

  if (m_PxSceneDesc) {
    delete m_PxSceneDesc;
  }

  if (m_PxPvd) {
    m_PxPvd->release();
  }

  PX_RELEASE(m_PxFoundation);

  YEAGER_DELETE(m_PhysXGeometryHandle);
  //YEAGER_DELETE(m_CharacterController);
  Yeager::Log(INFO, "PhysX Engine terminated!");
}

void PhysXHandle::StartSimulation(float deltaTime)
{
  m_PxScene->simulate(deltaTime);
}

void PhysXHandle::EndSimulation()
{
  m_PxScene->fetchResults(true);
}
