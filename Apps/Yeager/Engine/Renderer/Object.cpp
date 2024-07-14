#include "Object.h"
#include "../../Application.h"
#include "../Physics/PhysXActor.h"
#include "Animation/AnimationEngine.h"
#include "Importer.h"
using namespace Yeager;
using namespace physx;

void Yeager::SpawnCubeObject(Yeager::ApplicationCore* application, const String& name, const Vector3& position,
                             const Vector3& rotation, const Vector3& scale, const ObjectPhysicsType::Enum physics)
{
  auto object = std::make_shared<Object>(name, application);
  object->SetCanBeSerialize(false);
  object->GenerateGeometryTexture(application->GetDefaults()->GetTexture().get());
  if (physics == ObjectPhysicsType::eSTATIC_BODY) {
    object->GenerateObjectGeometry(ObjectGeometryType::eCUBE, ObjectPhysXCreationStatic(position, rotation, scale));
  } else {
    object->GenerateObjectGeometry(ObjectGeometryType::eCUBE, ObjectPhysXCreationDynamic(position, rotation, scale));
  }
  application->GetScene()->GetObjects()->push_back(object);
}

void Yeager::SpawnSphereObject(Yeager::ApplicationCore* application, const String& name, const Vector3& position,
                               const Vector3& rotation, const Vector3& scale, const ObjectPhysicsType::Enum physics)
{
  auto object = std::make_shared<Object>(name, application);
  object->SetCanBeSerialize(false);
  object->GenerateGeometryTexture(application->GetDefaults()->GetTexture().get());
  if (physics == ObjectPhysicsType::eSTATIC_BODY) {
    object->GenerateObjectGeometry(ObjectGeometryType::eSPHERE, ObjectPhysXCreationStatic(position, rotation, scale));
  } else {
    object->GenerateObjectGeometry(ObjectGeometryType::eSPHERE, ObjectPhysXCreationDynamic(position, rotation, scale));
  }
  application->GetScene()->GetObjects()->push_back(object);
}

String Yeager::ObjectGeometryTypeToString(ObjectGeometryType::Enum type)
{
  switch (type) {
    case ObjectGeometryType::eCUBE:
      return "Cube";
    case ObjectGeometryType::eSPHERE:
      return "Sphere";
    case ObjectGeometryType::eTRIANGLE:
      return "Triangle";
    case ObjectGeometryType::eCUSTOM:
      return "Custom";
    default:
      return "ERROR_CANNOT_FIND_TYPE";
  }
}

String ObjectInstancedType::EnumToString(ObjectInstancedType::Enum e)
{
  switch (e) {
    case ObjectInstancedType::eINSTANCED:
      return "Instanced";
    case ObjectInstancedType::eNON_INSTACED:
      return "Non Instanced";
    default:
      return "NULL";
  }
}

ObjectGeometryType::Enum Yeager::StringToObjectGeometryType(const String& str)
{
  switch (StringToInteger(str.c_str())) {
    case StringToInteger("Cube"):
      return ObjectGeometryType::eCUBE;
    case StringToInteger("Sphere"):
      return ObjectGeometryType::eSPHERE;
    case StringToInteger("Triangle"):
      return ObjectGeometryType::eTRIANGLE;
    case StringToInteger("Custom"):
      return ObjectGeometryType::eCUSTOM;
    default:
      Yeager::Log(ERROR,
                  "StringToObjectGeometryType error! Cannot find the type of the string given! Returning Custom!");
      return ObjectGeometryType::eCUSTOM;
  }
}

Object::Object(String name, ApplicationCore* application)
    : GameEntity(EntityObjectType::OBJECT, application, name),
      m_InstancedType(ObjectInstancedType::eNON_INSTACED),
      m_Actor(std::make_shared<PhysXActor>(m_Application, this)),
      m_ThreadImporter(std::make_shared<ImporterThreaded>(name, m_Application))
{
  BuildNode(m_Application->GetScene()->GetRootNode());
}

Object::Object(String name, ApplicationCore* application, GLuint amount)
    : GameEntity(EntityObjectType::OBJECT, application, name),
      m_InstancedObjs(amount),
      m_InstancedType(ObjectInstancedType::eINSTANCED),
      m_Actor(std::make_shared<PhysXActor>(m_Application, this)),
      m_ThreadImporter(std::make_shared<ImporterThreaded>(name, m_Application))
{
  m_Props.reserve(amount);
  BuildNode(m_Application->GetScene()->GetRootNode());
}

AnimatedObject::AnimatedObject(String name, ApplicationCore* application)
    : Object(name, application), m_ThreadImporter(std::make_shared<ImporterThreadedAnimated>(name, m_Application))
{
  SetEntityType(EntityObjectType::OBJECT_ANIMATED);
}

AnimatedObject::AnimatedObject(String name, ApplicationCore* application, GLuint amount)
    : Object(name, application, amount),
      m_ThreadImporter(std::make_shared<ImporterThreadedAnimated>(name, m_Application))
{
  SetEntityType(EntityObjectType::OBJECT_INSTANCED_ANIMATED);
}

void Object::BuildProps(const std::vector<std::shared_ptr<Transformation3D>>& transformations, Shader* shader)
{
  if (transformations.size() > m_InstancedObjs)
    Yeager::Log(WARNING,
                "Trying to build instanced props with a std::vector bigger that the amount that was declared! {} ",
                m_Name);
  m_Props = transformations;
  shader->UseShader();
  /* The number of instances can differ from the actual number of props that exists*/
  if (m_Props.size() == m_InstancedObjs) {
    for (Uint x = 0; x < m_InstancedObjs; x++) {
      Matrix4 model = Transformation3D::Apply(*(m_Props.at(x).get()));
      shader->SetMat4("matrices[" + std::to_string(x) + "]", model);
    }
  }
}

Object::~Object()
{
  if (m_InstancedType == ObjectInstancedType::eINSTANCED) {
    for (auto& trans : m_Props)
      trans.reset();
  }

  if (!m_ThreadImporter->IsThreadFinish()) {
    Yeager::Log(INFO, "Awaiting Object {} thread to finish", GetName());
    if (m_ThreadImporter->GetThreadPtr()->joinable()) {
      m_ThreadImporter->GetThreadPtr()->join();
    }
  }
  m_ThreadImporter.reset();
  if (m_ObjectDataLoaded) {
    if (m_GeometryType == ObjectGeometryType::eCUSTOM) {
      m_GeometryData.Renderer.DeleteBuffers();
    } else {
      for (auto& mesh : m_ModelData.Meshes) {
        DeleteMeshGLBuffers(&mesh);
      }
    }
    m_Actor.reset();
    Yeager::Log(INFO, "Destroying object {}", m_Name);
  }
}

AnimatedObject::~AnimatedObject()
{
  m_AnimationEngine.reset();
  m_ThreadImporter.reset();
}

void Yeager::DeleteMeshGLBuffers(ObjectMeshData* mesh)
{
  mesh->Renderer.DeleteBuffers();
}

std::vector<GLfloat> Yeager::ExtractVerticesFromEveryMesh(ObjectModelData* model)
{
  std::vector<GLfloat> vertices;
  for (auto& mesh : model->Meshes) {

    for (auto& vertex : mesh.Vertices) {
      vertices.push_back(vertex.Position.x);
      vertices.push_back(vertex.Position.y);
      vertices.push_back(vertex.Position.z);
      vertices.push_back(vertex.Normals.x);
      vertices.push_back(vertex.Normals.y);
      vertices.push_back(vertex.Normals.z);
      vertices.push_back(vertex.TextureCoords.x);
      vertices.push_back(vertex.TextureCoords.y);
    }
  }
  return vertices;
}

std::vector<Vector3> Yeager::ExtractVerticesPositionToVector(ObjectModelData* model)
{
  std::vector<Vector3> Positions;
  for (const auto& mesh : model->Meshes) {
    for (const auto& vertex : mesh.Vertices) {
      Vector3 vec(vertex.Position.x, vertex.Position.y, vertex.Position.z);
      Positions.push_back(vec);
    }
  }
  return Positions;
}

void AnimatedObject::UpdateAnimation(float delta)
{
  if (m_ObjectDataLoaded && m_Render) {
    m_AnimationEngine->UpdateAnimation(delta);
  }
}

void AnimatedObject::BuildAnimationMatrices(Shader* shader)
{
  if (m_ObjectDataLoaded && m_Render) {
    shader->UseShader();
    auto transform = m_AnimationEngine->GetFinalBoneMatrices();
    for (int x = 0; x < transform.size(); x++) {
      shader->SetMat4("finalBonesMatrices[" + std::to_string(x) + "]", transform.at(x));
    }
  }
}

void Yeager::DrawSeparateMesh(ObjectMeshData* mesh, Yeager::Shader* shader)
{
  shader->UseShader();
  Uint diffuseNum = 1;
  Uint specularNum = 1;

  for (Uint x = 0; x < mesh->Textures.size(); x++) {
    glActiveTexture(GL_TEXTURE0 + x);
    String number;
    String name = mesh->Textures[x]->GetName();
    if (name == "texture_diffuse") {
      number = std::to_string(diffuseNum++);
    } else if (name == "texture_specular") {
      number = std::to_string(specularNum++);
    }
    String mat = ("material." + name + number).c_str();
    shader->SetInt(mat, x);
    mesh->Textures[x]->BindTexture();
  }

  mesh->Renderer.BindVertexArray();
  glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh->Indices.size()), GL_UNSIGNED_INT, YEAGER_NULLPTR);
  mesh->Renderer.UnbindVertexArray();

  MaterialTexture2D::Unbind2DTextures();
}

void Yeager::DrawSeparateInstancedMesh(ObjectMeshData* mesh, Yeager::Shader* shader, int amount)
{
  Uint diffuseNum = 1;
  Uint specularNum = 1;

  for (Uint x = 0; x < mesh->Textures.size(); x++) {
    glActiveTexture(GL_TEXTURE0 + x);
    String number;
    String name = mesh->Textures[x]->GetName();
    if (name == "texture_diffuse") {
      number = std::to_string(diffuseNum++);
    } else if (name == "texture_specular") {
      number = std::to_string(specularNum++);
    }
    shader->SetInt((name + number).c_str(), x);
    mesh->Textures[x]->BindTexture();
  }

  mesh->Renderer.BindVertexArray();
  mesh->Renderer.DrawInstanced(GL_TRIANGLES, static_cast<GLsizei>(mesh->Indices.size()), GL_UNSIGNED_INT,
                               YEAGER_NULLPTR, amount);
  mesh->Renderer.UnbindVertexArray();

  MaterialTexture2D::Unbind2DTextures();
}

bool Object::GenerateObjectGeometry(ObjectGeometryType::Enum geometry, const ObjectPhysXCreationBase& physics)
{
  if (!m_ObjectDataLoaded) {
    /* Defines geometry and pull all the vertices and indices */
    m_GeometryType = geometry;
    switch (geometry) {
      case ObjectGeometryType::eCUBE:
        m_GeometryData.Vertices = GenerateCubeVertices();
        m_GeometryData.Indices = GenerateCubeIndices();
        break;
      case ObjectGeometryType::eSPHERE:
        m_GeometryData.Vertices = GenerateSphereVertices(50, 50);
        m_GeometryData.Indices = GenerateSphereIndices(50, 50);
        break;
      default:
        Yeager::Log(ERROR, "Cannot generate geometry, invalid type! model {}", m_Name);
        return false;
    }
    m_PhysicsType = physics.Type;
    physics.ApplyToObjectTransformation(&m_EntityTransformation);
    m_Actor->BuildActor(physics);
    Setup();
    m_ObjectDataLoaded = true;

    Yeager::LogDebug(INFO, "Success in loading geometry {}", m_Name);

    return true;

  } else {
    Yeager::Log(WARNING, "Object data already loaded! Trying to load geometry to model {}", m_Name);
    return false;
  }
}

bool Object::ImportObjectFromFile(Cchar path, const ObjectCreationConfiguration configuration, bool flip_image)
{
  Path = path;

  if (!m_ObjectDataLoaded) {
    Importer imp(m_Name, m_Application);

    m_ModelData = imp.Import(path, configuration, flip_image);

    if (!m_ModelData.SuccessfulLoaded) {
      Yeager::Log(ERROR, "Cannot load imported model data, model {} path {}", m_Name, path);
      m_ObjectDataLoaded = false;
      return false;
    }

    m_GeometryType = ObjectGeometryType::eCUSTOM;
    m_ObjectDataLoaded = true;

    Setup();

    for (auto& tex : m_ModelData.TexturesLoaded) {
      m_EntityLoadedTextures.push_back(&tex->first);
    }

    Yeager::LogDebug(INFO, "Success in loading mode {} path {}", m_Name, path);

    return true;

  } else {
    Yeager::Log(WARNING, "Model data already loaded! Trying to load imported model to model {} path {}", m_Name, path);
    return false;
  }
}

bool Object::ThreadImportObjectFromFile(Cchar path, const ObjectCreationConfiguration configuration, bool flip_image)
{
  Path = path;

  if (!m_ObjectDataLoaded) {
    m_ThreadImporter->ThreadImport(path, configuration, flip_image);
    std::pair<ImporterThreaded*, Yeager::Object*> thread;
    thread.first = m_ThreadImporter.get();
    thread.second = this;
    m_Application->GetScene()->GetThreadImporters()->push_back(thread);
    return true;
  } else {
    Yeager::Log(WARNING, "Model data already loaded! Trying to load imported model to model {} path {}", m_Name, path);
    return false;
  }
}

void Object::ThreadLoadIncompleteTextures()
{
  for (auto& tex : m_ModelData.TexturesLoaded) {
    if (tex->first.GetTextureDataHandle()->ImcompletedID) {
      tex->first.GenerateFromData(tex->second);
      tex->first.GetTextureDataHandle()->ImcompletedID = false;
      if (tex->second != YEAGER_NULLPTR) {
        delete tex->second;
      }
    }
  }
}

void Object::ThreadSetup()
{

  m_ModelData = m_ThreadImporter->GetValue();
  m_ThreadImporter->GetThreadPtr()->join();

  if (!m_ModelData.SuccessfulLoaded) {
    Yeager::Log(ERROR, "Cannot load imported model data, model {}", m_Name);
    m_ObjectDataLoaded = false;
    return;
  }

  ThreadLoadIncompleteTextures();

  m_GeometryType = ObjectGeometryType::eCUSTOM;
  m_ObjectDataLoaded = true;
  Setup();

  for (auto& tex : m_ModelData.TexturesLoaded) {
    m_EntityLoadedTextures.push_back(&tex->first);
  }

  Yeager::LogDebug(INFO, "Success in loading mode {}", m_Name);
}

bool AnimatedObject::ThreadImportObjectFromFile(Cchar path, const ObjectCreationConfiguration configuration,
                                                bool flip_image)
{
  Path = path;

  if (!m_ObjectDataLoaded) {
    m_ThreadImporter->ThreadImport(path, configuration, flip_image);
    std::pair<ImporterThreadedAnimated*, Yeager::AnimatedObject*> thread;
    thread.first = m_ThreadImporter.get();
    thread.second = this;
    m_Application->GetScene()->GetThreadAnimatedImporters()->push_back(thread);
    return true;
  } else {
    Yeager::Log(WARNING, "Model Animated data already loaded! Trying to load imported model to model {} path {}",
                m_Name, path);
    return false;
  }
}

void AnimatedObject::ThreadLoadIncompleteTextures()
{
  for (auto& tex : m_ModelData.TexturesLoaded) {
    if (tex->first.GetTextureDataHandle()->ImcompletedID) {
      tex->first.GenerateFromData(tex->second);
      if (tex->second != YEAGER_NULLPTR) {
        delete tex->second;
      }
    }
  }
}

void AnimatedObject::ThreadSetup()
{
  m_ModelData = m_ThreadImporter->GetValue();
  m_ThreadImporter->GetThreadPtr()->join();

  if (!m_ModelData.SuccessfulLoaded) {
    Yeager::Log(ERROR, "Cannot load imported model data, model {}", m_Name);
    m_ObjectDataLoaded = false;
    return;
  }

  ThreadLoadIncompleteTextures();

  m_GeometryType = ObjectGeometryType::eCUSTOM;
  m_ObjectDataLoaded = true;

  Setup();

  for (auto& tex : m_ModelData.TexturesLoaded) {
    m_EntityLoadedTextures.push_back(&tex->first);
  }

  BuildAnimation(Path);

#ifdef YEAGER_DEBUG
  Yeager::Log(INFO, "Success in loading mode {}", m_Name);
#endif
}

void Object::DrawInstancedGeometry(Yeager::Shader* shader)
{
  m_GeometryData.Renderer.BindVertexArray();

  m_GeometryData.Renderer.DrawInstanced(GL_TRIANGLES, static_cast<unsigned int>(m_GeometryData.Indices.size()),
                                        GL_UNSIGNED_INT, 0, m_InstancedObjs);

  m_GeometryData.Renderer.UnbindVertexArray();
  glActiveTexture(GL_TEXTURE0);
}

void Object::DrawGeometry(Yeager::Shader* shader)
{
  if (m_GeometryData.Texture) {
    glActiveTexture(GL_TEXTURE0);
    shader->SetInt("material.texture_diffuse1", 0);
    glBindTexture(GL_TEXTURE_2D, m_GeometryData.Texture->GetTextureID());
  }

  m_GeometryData.Renderer.BindVertexArray();

  m_GeometryData.Renderer.Draw(GL_TRIANGLES, static_cast<unsigned int>(m_GeometryData.Indices.size()), GL_UNSIGNED_INT,
                               0);
  m_GeometryData.Renderer.UnbindVertexArray();
  MaterialTexture2D::Unbind2DTextures();
}

void Object::DrawModel(Yeager::Shader* shader)
{
  for (auto& mesh : m_ModelData.Meshes) {
    if (m_InstancedType == ObjectInstancedType::eNON_INSTACED) {
      DrawSeparateMesh(&mesh, shader);
    } else {
      DrawSeparateInstancedMesh(&mesh, shader, m_InstancedObjs);
    }
  }
}

void Object::ProcessOnScreenProprieties()
{
  if (!m_OnScreenProprieties.m_CullingEnabled)
    glDisable(GL_CULL_FACE);

  switch (m_OnScreenProprieties.m_PolygonMode) {
    case RenderingGLPolygonMode::eLINES:
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      break;
    case RenderingGLPolygonMode::ePOINTS:
      glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
      break;
    case RenderingGLPolygonMode::eFILL:
    default:
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }
}
void Object::PosProcessOnScreenProprieties()
{
  if (!m_OnScreenProprieties.m_CullingEnabled)
    glEnable(GL_CULL_FACE);

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Object::Draw(Yeager::Shader* shader, float delta)
{

  ProcessOnScreenProprieties();

  if (m_ObjectDataLoaded && m_Render) {

    shader->UseShader();
    if (m_PhysicsType == ObjectPhysicsType::eDYNAMIC_BODY)
      m_Actor->ProcessTransformation(delta);
    ApplyTransformation(shader);

    if (m_GeometryType == ObjectGeometryType::eCUSTOM) {
      DrawModel(shader);
    } else {
      if (m_InstancedType == ObjectInstancedType::eNON_INSTACED) {
        DrawGeometry(shader);
      } else {
        DrawInstancedGeometry(shader);
      }
    }
  }

  PosProcessOnScreenProprieties();
}

void Object::Setup()
{

  if (m_GeometryType == ObjectGeometryType::eCUSTOM) {

    for (auto& mesh : m_ModelData.Meshes) {
      mesh.Renderer.GenBuffers();
      mesh.Renderer.BindBuffers();

      mesh.Renderer.BufferData(GL_ARRAY_BUFFER, mesh.Vertices.size() * sizeof(ObjectVertexData), &mesh.Vertices[0],
                               GL_STATIC_DRAW);
      mesh.Renderer.BufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.Indices.size() * sizeof(unsigned int), &mesh.Indices[0],
                               GL_STATIC_DRAW);

      mesh.Renderer.VertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ObjectVertexData), (void*)0);

      mesh.Renderer.VertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(ObjectVertexData),
                                        (void*)offsetof(ObjectVertexData, Normals));

      mesh.Renderer.VertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(ObjectVertexData),
                                        (void*)offsetof(ObjectVertexData, TextureCoords));

      mesh.Renderer.UnbindBuffers();
    }
  } else {

    m_GeometryData.Renderer.GenBuffers();
    m_GeometryData.Renderer.BindBuffers();

    m_GeometryData.Renderer.BufferData(GL_ARRAY_BUFFER, m_GeometryData.Vertices.size() * sizeof(GLfloat),
                                       &m_GeometryData.Vertices[0], GL_STATIC_DRAW);

    m_GeometryData.Renderer.BufferData(GL_ELEMENT_ARRAY_BUFFER, m_GeometryData.Indices.size() * sizeof(GLuint),
                                       &m_GeometryData.Indices[0], GL_STATIC_DRAW);

    m_GeometryData.Renderer.VertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);

    m_GeometryData.Renderer.VertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
                                                (void*)(3 * sizeof(GLfloat)));

    m_GeometryData.Renderer.VertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
                                                (void*)(6 * sizeof(GLfloat)));
    m_GeometryData.Renderer.UnbindBuffers();
  }
}

void Object::GenerateGeometryTexture(MaterialTexture2D* texture)
{
  m_GeometryData.Texture = texture;
}

void AnimatedObject::BuildAnimation(String path)
{
  m_AnimationEngine = std::make_shared<AnimationEngine>();
  m_AnimationEngine->Initialize();
  m_AnimationEngine->LoadAnimationsFromFile(path, this);
}

bool AnimatedObject::ImportObjectFromFile(Cchar path, const ObjectCreationConfiguration configuration, bool flip_image)
{
  Path = path;

  if (!m_ObjectDataLoaded) {
    Importer imp(m_Name, m_Application);
    m_ModelData = imp.ImportAnimated(path, configuration, flip_image);

    if (!m_ModelData.SuccessfulLoaded) {
      Yeager::Log(ERROR, "Cannot load imported model data, model {} path {}", m_Name, path);
      m_ObjectDataLoaded = false;
      return false;
    }
    m_GeometryType = ObjectGeometryType::eCUSTOM;
    m_ObjectDataLoaded = true;

    Setup();

    for (auto& tex : m_ModelData.TexturesLoaded) {
      m_EntityLoadedTextures.push_back(&tex->first);
    }

    Yeager::Log(INFO, "Success in loading mode {} path {}", m_Name, path);
    return true;
  } else {
    Yeager::Log(WARNING, "Model data already loaded! Trying to load imported model to model {} path {}", m_Name, path);
    return false;
  }
}

void AnimatedObject::Setup()
{

  for (auto& mesh : m_ModelData.Meshes) {
    mesh.Renderer.GenBuffers();
    mesh.Renderer.BindBuffers();

    mesh.Renderer.BufferData(GL_ARRAY_BUFFER, mesh.Vertices.size() * sizeof(AnimatedVertexData), &mesh.Vertices[0],
                             GL_STATIC_DRAW);
    mesh.Renderer.BufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.Indices.size() * sizeof(unsigned int), &mesh.Indices[0],
                             GL_STATIC_DRAW);

    mesh.Renderer.VertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(AnimatedVertexData), (void*)0);

    mesh.Renderer.VertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(AnimatedVertexData),
                                      (void*)offsetof(AnimatedVertexData, Normals));

    mesh.Renderer.VertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(AnimatedVertexData),
                                      (void*)offsetof(AnimatedVertexData, TextureCoords));

    mesh.Renderer.VertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(AnimatedVertexData),
                                      (void*)offsetof(AnimatedVertexData, Tangent));

    mesh.Renderer.VertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(AnimatedVertexData),
                                      (void*)offsetof(AnimatedVertexData, BiTangent));

    mesh.Renderer.VertexAttribPointer(5, 4, GL_INT, GL_FALSE, sizeof(AnimatedVertexData),
                                      (void*)offsetof(AnimatedVertexData, BonesIDs));

    mesh.Renderer.VertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(AnimatedVertexData),
                                      (void*)offsetof(AnimatedVertexData, Weights));

    mesh.Renderer.UnbindBuffers();
  }
}

void AnimatedObject::Draw(Shader* shader)
{
  ProcessOnScreenProprieties();

  if (m_ObjectDataLoaded && m_Render) {
    shader->UseShader();
    if (m_InstancedType == ObjectInstancedType::eNON_INSTACED)
      ApplyTransformation(shader);
    DrawMeshes(shader);
  }

  PosProcessOnScreenProprieties();
}

void AnimatedObject::DrawMeshes(Shader* shader)
{
  for (auto& mesh : m_ModelData.Meshes) {
    Uint diffuseNum = 1;
    Uint specularNum = 1;
    Uint normalNum = 1;
    Uint heightNum = 1;

    for (Uint x = 0; x < mesh.Textures.size(); x++) {
      glActiveTexture(GL_TEXTURE0 + x);
      String number;
      String name = mesh.Textures[x]->GetName();
      if (name == "texture_diffuse") {
        number = std::to_string(diffuseNum++);
      } else if (name == "texture_specular") {
        number = std::to_string(specularNum++);
      } else if (name == "texture_normal") {
        number = std::to_string(normalNum++);
      } else if (name == "texture_height") {
        number = std::to_string(heightNum++);
      }
      shader->SetInt(("material." + name + number).c_str(), x);
      glBindTexture(GL_TEXTURE_2D, mesh.Textures[x]->GetTextureID());
    }

    mesh.Renderer.BindVertexArray();
    if (m_InstancedType == ObjectInstancedType::eNON_INSTACED) {
      mesh.Renderer.Draw(GL_TRIANGLES, static_cast<GLsizei>(mesh.Indices.size()), GL_UNSIGNED_INT, YEAGER_NULLPTR);
    } else {
      mesh.Renderer.DrawInstanced(GL_TRIANGLES, static_cast<GLsizei>(mesh.Indices.size()), GL_UNSIGNED_INT,
                                  YEAGER_NULLPTR, m_InstancedObjs);
    }

    mesh.Renderer.UnbindVertexArray();
    MaterialTexture2D::Unbind2DTextures();
  }
}

// clang-format off
std::vector<GLfloat> Yeager::GenerateCubeVertices()
{
  return std::vector<GLfloat>{

        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f,  0.0f, 0.0f,  // A 0
        0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f, 1.0f, 0.0f,  // B 1
        0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f, 1.0f, 1.0f,  // C 2
        -0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f,  0.0f, 1.0f,  // D 3
        -0.5f, -0.5f,  0.5f, 0.0f, 0.0f, -1.0f,  0.0f, 0.0f,  // E 4
        0.5f, -0.5f,  0.5f,  0.0f, 0.0f, -1.0f, 1.0f, 0.0f,   // F 5

        0.5f,  0.5f,  0.5f,  0.0f, 0.0f, -1.0f, 1.0f, 1.0f,   // G 6
        -0.5f,  0.5f,  0.5f, 0.0f, 0.0f, -1.0f,  0.0f, 1.0f,   // H 7
        -0.5f,  0.5f, -0.5f, -1.0f, 0.0f, 0.0f,  0.0f, 0.0f,  // D 8
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,  1.0f, 0.0f,  // A 9
        -0.5f, -0.5f,  0.5f, -1.0f, 0.0f, 0.0f,  1.0f, 1.0f,  // E 10
        -0.5f,  0.5f,  0.5f, -1.0f, 0.0f, 0.0f,  0.0f, 1.0f,  // H 11

        0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f, 0.0f, 0.0f,   // B 12
        0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f, 1.0f, 0.0f,   // C 13
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f, 1.0f, 1.0f,   // G 14
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f,   // F 15
        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,  0.0f, 0.0f,  // A 16
        0.5f, -0.5f, -0.5f,  0.0f, -1.0f, 0.0f, 1.0f, 0.0f,   // B 17
        
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f, 0.0f, 1.0f, 1.0f,   // F 18
        -0.5f, -0.5f,  0.5f, 0.0f, -1.0f, 0.0f,  0.0f, 1.0f,  // E 19
        0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,  // C 20
        -0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f,  1.0f, 0.0f,  // D 21
        -0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f,  1.0f, 1.0f,  // H 22
        0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f };
}  // clang-format on

std::vector<GLuint> Yeager::GenerateCubeIndices()
{
  return std::vector<GLuint>{// front and back
                             0, 3, 2, 2, 1, 0, 4, 5, 6, 6, 7, 4,
                             // left and right
                             11, 8, 9, 9, 10, 11, 12, 13, 14, 14, 15, 12,
                             // bottom and top
                             16, 17, 18, 18, 19, 16, 20, 21, 22, 22, 23, 20};
}

std::vector<GLfloat> Yeager::GenerateSphereVertices(int stackCount, int sectorCount)
{
  std::vector<GLfloat> vertices;
  int radius = 10;

  float x, y, z, xy;
  float nx, ny, nz, lenghtInv = 1.0f / radius;
  float s, t;

  float sectorStep = 2 * YEAGER_PI / sectorCount;
  float stackStep = YEAGER_PI / stackCount;
  float sectorAngle, stackAngle;

  for (int i = 0; i <= stackCount; ++i) {
    stackAngle = YEAGER_PI / 2 - i * stackStep;
    xy = radius * cosf(stackAngle);
    z = radius * sinf(stackAngle);

    for (int j = 0; j <= sectorCount; ++j) {
      sectorAngle = j * sectorStep;

      x = xy * cosf(sectorAngle);
      y = xy * sinf(sectorAngle);
      vertices.push_back(x);
      vertices.push_back(y);
      vertices.push_back(z);

      nx = x * lenghtInv;
      ny = y * lenghtInv;
      nz = z * lenghtInv;
      vertices.push_back(nx);
      vertices.push_back(ny);
      vertices.push_back(nz);

      s = (float)j / sectorCount;
      t = (float)i / stackCount;
      vertices.push_back(s);
      vertices.push_back(t);
    }
  }
  return vertices;
}

std::vector<GLuint> Yeager::GenerateSphereIndices(int stackCount, int sectorCount)
{
  std::vector<GLuint> indices;
  int k1, k2;
  for (int x = 0; x < stackCount; ++x) {
    k1 = x * (sectorCount + 1);
    k2 = k1 + sectorCount + 1;
    for (int y = 0; y < sectorCount; ++y, ++k1, ++k2) {
      if (x != 0) {
        indices.push_back(k1);
        indices.push_back(k2);
        indices.push_back(k1 + 1);
      }

      if (x != (stackCount - 1)) {
        indices.push_back(k1 + 1);
        indices.push_back(k2);
        indices.push_back(k2 + 1);
      }
    }
  }
  return indices;
}
