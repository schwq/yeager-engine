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
  auto object = std::make_shared<Yeager::Object>(name, application);
  object->SetCanBeSerialize(false);
  object->GenerateGeometryTexture(application->monarch.get());
  object->GenerateObjectGeometry(ObjectGeometryType::eCUBE, ObjectPhysXCreationStatic(position, rotation, scale));
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

Object::Object(String name, ApplicationCore* application) : GameEntity(EntityObjectType::OBJECT, application, name)
{
  m_InstancedType = ObjectInstancedType::eNON_INSTACED;
  m_Actor = new PhysXActor(m_Application, this);

  m_ThreadImporter = new ImporterThreaded("Object", m_Application);
}

Object::Object(String name, ApplicationCore* application, GLuint amount)
    : GameEntity(EntityObjectType::OBJECT, application, name), m_InstancedObjs(amount)
{
  m_InstancedType = ObjectInstancedType::eINSTANCED;
  m_Props.reserve(amount);
  m_Actor = new PhysXActor(m_Application, this);

  m_ThreadImporter = new ImporterThreaded("Object", m_Application);
}

void Object::BuildProps(const std::vector<Transformation*>& transformations, Shader* shader)
{
  if (transformations.size() > m_InstancedObjs)
    Yeager::Log(WARNING,
                "Trying to build instanced props with a std::vector bigger that the amount that was declared! {} ",
                m_Name);
  m_Props = transformations;
  shader->UseShader();
  /* The number of instances can differ from the actual number of props that exists*/
  if (m_Props.size() == m_InstancedObjs) {
    for (unsigned int x = 0; x < m_InstancedObjs; x++) {
      Yeager::ApplyTransformation(m_Props.at(x));
      shader->SetMat4("matrices[" + std::to_string(x) + "]", m_Props.at(x)->model);
    }
  }
}

Object::~Object()
{
  if (m_InstancedType == ObjectInstancedType::eINSTANCED) {
    for (auto& trans : m_Props)
      YEAGER_DELETE(trans);
  }

  if (!m_ThreadImporter->IsThreadFinish()) {
    Yeager::Log(INFO, "Awaiting Object {} thread to finish", GetName());
    if (m_ThreadImporter->GetThreadPtr()->joinable()) {
      m_ThreadImporter->GetThreadPtr()->join();
    }
  }
  YEAGER_DELETE(m_ThreadImporter);
  if (m_ObjectDataLoaded) {
    if (m_GeometryType == ObjectGeometryType::eCUSTOM) {
      glDeleteVertexArrays(1, &m_GeometryData.m_Vao);
      glDeleteBuffers(1, &m_GeometryData.m_Vbo);
      glDeleteBuffers(1, &m_GeometryData.m_Ebo);
    } else {
      for (auto& mesh : m_ModelData.Meshes) {
        DeleteMeshGLBuffers(&mesh);
      }
    }
    YEAGER_DELETE(m_Actor);
    Yeager::Log(INFO, "Destroying object {}", m_Name);
  }
}

AnimatedObject::~AnimatedObject()
{
  YEAGER_DELETE(m_Animation)
  YEAGER_DELETE(m_AnimationEngine)
  YEAGER_DELETE(m_ThreadImporter)
}

void Yeager::DeleteMeshGLBuffers(ObjectMeshData* mesh)
{
  glDeleteVertexArrays(1, &mesh->m_Vao);
  glDeleteBuffers(1, &mesh->m_Vbo);
  glDeleteBuffers(1, &mesh->m_Ebo);
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
  m_AnimationEngine->UpdateAnimation(delta);
}

void AnimatedObject::BuildAnimationMatrices(Shader* shader)
{
  shader->UseShader();
  auto transform = m_AnimationEngine->GetFinalBoneMatrices();
  for (int x = 0; x < transform.size(); x++) {
    shader->SetMat4("finalBonesMatrices[" + std::to_string(x) + "]", transform.at(x));
  }
}

void Yeager::DrawSeparateMesh(ObjectMeshData* mesh, Yeager::Shader* shader)
{
  shader->UseShader();
  unsigned int diffuseNum = 1;
  unsigned int specularNum = 1;

  for (unsigned int x = 0; x < mesh->Textures.size(); x++) {
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
    glBindTexture(GL_TEXTURE_2D, mesh->Textures[x]->GetTextureID());
  }

  glBindVertexArray(mesh->m_Vao);
  glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh->Indices.size()), GL_UNSIGNED_INT, YEAGER_NULLPTR);

  glBindVertexArray(0);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, NULL);
}

void Yeager::DrawSeparateInstancedMesh(ObjectMeshData* mesh, Yeager::Shader* shader, int amount)
{
  unsigned int diffuseNum = 1;
  unsigned int specularNum = 1;

  for (unsigned int x = 0; x < mesh->Textures.size(); x++) {
    glActiveTexture(GL_TEXTURE0 + x);
    String number;
    String name = mesh->Textures[x]->GetName();
    if (name == "texture_diffuse") {
      number = std::to_string(diffuseNum++);
    } else if (name == "texture_specular") {
      number = std::to_string(specularNum++);
    }
    shader->SetInt((name + number).c_str(), x);
    glBindTexture(GL_TEXTURE_2D, mesh->Textures[x]->GetTextureID());
  }

  glBindVertexArray(mesh->m_Vao);
  glDrawElementsInstanced(GL_TRIANGLES, static_cast<GLsizei>(mesh->Indices.size()), GL_UNSIGNED_INT, YEAGER_NULLPTR,
                          amount);

  glBindVertexArray(0);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, 0);
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

bool Object::ImportObjectFromFile(Cchar path, bool flip_image)
{
  Path = path;

  if (!m_ObjectDataLoaded) {
    Importer imp(m_Name, m_Application);

    m_ModelData = imp.Import(path, flip_image);

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

bool Object::ThreadImportObjectFromFile(Cchar path, bool flip_image)
{
  Path = path;

  if (!m_ObjectDataLoaded) {
    m_ThreadImporter->ThreadImport(path, flip_image);
    std::pair<ImporterThreaded*, Yeager::Object*> thread;
    thread.first = m_ThreadImporter;
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

bool AnimatedObject::ThreadImportObjectFromFile(Cchar path, bool flip_image)
{
  Path = path;

  if (!m_ObjectDataLoaded) {
    m_ThreadImporter->ThreadImport(path, flip_image);
    std::pair<ImporterThreadedAnimated*, Yeager::AnimatedObject*> thread;
    thread.first = m_ThreadImporter;
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
  glBindVertexArray(m_GeometryData.m_Vao);

  glDrawElementsInstanced(GL_TRIANGLES, static_cast<unsigned int>(m_GeometryData.Indices.size()), GL_UNSIGNED_INT, 0,
                          m_InstancedObjs);

  glBindVertexArray(0);
  glActiveTexture(GL_TEXTURE0);
}

void Object::DrawGeometry(Yeager::Shader* shader)
{
  if (m_GeometryData.Texture) {
    glActiveTexture(GL_TEXTURE0);
    shader->SetInt("material.texture_diffuse1", 0);
    glBindTexture(GL_TEXTURE_2D, m_GeometryData.Texture->GetTextureID());
  }

  glBindVertexArray(m_GeometryData.m_Vao);

  glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(m_GeometryData.Indices.size()), GL_UNSIGNED_INT, 0);

  glBindVertexArray(0);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, NULL);
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

void Object::Draw(Yeager::Shader* shader, float delta)
{
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
}

void Object::Setup()
{

  if (m_GeometryType == ObjectGeometryType::eCUSTOM) {

    for (auto& mesh : m_ModelData.Meshes) {
      glGenVertexArrays(1, &mesh.m_Vao);
      glGenBuffers(1, &mesh.m_Vbo);
      glGenBuffers(1, &mesh.m_Ebo);

      glBindVertexArray(mesh.m_Vao);
      glBindBuffer(GL_ARRAY_BUFFER, mesh.m_Vbo);

      glBufferData(GL_ARRAY_BUFFER, mesh.Vertices.size() * sizeof(ObjectVertexData), &mesh.Vertices[0], GL_STATIC_DRAW);

      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.m_Ebo);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.Indices.size() * sizeof(unsigned int), &mesh.Indices[0],
                   GL_STATIC_DRAW);

      glEnableVertexAttribArray(0);
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ObjectVertexData), (void*)0);

      glEnableVertexAttribArray(1);
      glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(ObjectVertexData),
                            (void*)offsetof(ObjectVertexData, Normals));

      glEnableVertexAttribArray(2);
      glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(ObjectVertexData),
                            (void*)offsetof(ObjectVertexData, TextureCoords));

      glBindVertexArray(0);
    }
  } else {

    glGenVertexArrays(1, &m_GeometryData.m_Vao);
    glGenBuffers(1, &m_GeometryData.m_Vbo);
    glGenBuffers(1, &m_GeometryData.m_Ebo);

    glBindVertexArray(m_GeometryData.m_Vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_GeometryData.m_Vbo);
    glBufferData(GL_ARRAY_BUFFER, m_GeometryData.Vertices.size() * sizeof(GLfloat), &m_GeometryData.Vertices[0],
                 GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_GeometryData.m_Ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_GeometryData.Indices.size() * sizeof(GLuint), &m_GeometryData.Indices[0],
                 GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
  }
}

void Object::GenerateGeometryTexture(MaterialTexture2D* texture)
{
  m_GeometryData.Texture = texture;
}

AnimatedObject::AnimatedObject(String name, ApplicationCore* application) : Object(name, application)
{
  SetEntityType(EntityObjectType::OBJECT_ANIMATED);
  m_ThreadImporter = new ImporterThreadedAnimated("ObjectAnimated", m_Application);
}

AnimatedObject::AnimatedObject(String name, ApplicationCore* application, GLuint amount)
    : Object(name, application, amount)
{
  SetEntityType(EntityObjectType::OBJECT_ANIMATED);
  m_ThreadImporter = new ImporterThreadedAnimated("ObjectAnimated", m_Application);
}

void AnimatedObject::BuildAnimation(String path)
{
  m_Animation = new Animation(path, this);
  m_AnimationEngine = new AnimationEngine(m_Animation);
}

bool AnimatedObject::ImportObjectFromFile(Cchar path, bool flip_image)
{
  Path = path;

  if (!m_ObjectDataLoaded) {
    Importer imp(m_Name, m_Application);
    m_ModelData = imp.ImportAnimated(path, flip_image);

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
    glGenVertexArrays(1, &mesh.m_Vao);
    glGenBuffers(1, &mesh.m_Vbo);
    glGenBuffers(1, &mesh.m_Ebo);

    glBindVertexArray(mesh.m_Vao);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.m_Vbo);

    glBufferData(GL_ARRAY_BUFFER, mesh.Vertices.size() * sizeof(AnimatedVertexData), &mesh.Vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.m_Ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.Indices.size() * sizeof(unsigned int), &mesh.Indices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(AnimatedVertexData), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(AnimatedVertexData),
                          (void*)offsetof(AnimatedVertexData, Normals));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(AnimatedVertexData),
                          (void*)offsetof(AnimatedVertexData, TextureCoords));

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(AnimatedVertexData),
                          (void*)offsetof(AnimatedVertexData, Tangent));
    // vertex bitangent
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(AnimatedVertexData),
                          (void*)offsetof(AnimatedVertexData, BiTangent));
    // ids
    glEnableVertexAttribArray(5);
    glVertexAttribIPointer(5, 4, GL_INT, sizeof(AnimatedVertexData), (void*)offsetof(AnimatedVertexData, BonesIDs));

    // weights
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(AnimatedVertexData),
                          (void*)offsetof(AnimatedVertexData, Weights));

    glBindVertexArray(0);
  }
}

void AnimatedObject::Draw(Shader* shader)
{
  if (m_ObjectDataLoaded && m_Render) {
    shader->UseShader();
    if (m_InstancedType == ObjectInstancedType::eNON_INSTACED)
      ApplyTransformation(shader);
    DrawMeshes(shader);
  }
}

void AnimatedObject::DrawMeshes(Shader* shader)
{
  for (auto& mesh : m_ModelData.Meshes) {
    unsigned int diffuseNum = 1;
    unsigned int specularNum = 1;
    unsigned int normalNum = 1;
    unsigned int heightNum = 1;

    for (unsigned int x = 0; x < mesh.Textures.size(); x++) {
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

    glBindVertexArray(mesh.m_Vao);
    if (m_InstancedType == ObjectInstancedType::eNON_INSTACED) {
      glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh.Indices.size()), GL_UNSIGNED_INT, YEAGER_NULLPTR);
    } else {
      glDrawElementsInstanced(GL_TRIANGLES, static_cast<GLsizei>(mesh.Indices.size()), GL_UNSIGNED_INT, YEAGER_NULLPTR,
                              m_InstancedObjs);
    }

    glBindVertexArray(0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
  }
}

// clang-format off
std::vector<GLfloat> Yeager::GenerateCubeVertices()
{
  return std::vector<GLfloat>{
      -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f, 0.0f, 0.5f,  -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 1.0f, 0.0f,
      0.5f,  0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f, 1.0f, 1.0f, 0.5f,  0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f, 1.0f, 1.0f,
      -0.5f, 0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f, 0.0f,

      -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.5f,  -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
      0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f, 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
      -0.5f, 0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f, -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,

      -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,  1.0f, 0.0f, -0.5f, 0.5f,  -0.5f, -1.0f, 0.0f,  0.0f,  1.0f, 1.0f,
      -0.5f, -0.5f, -0.5f, -1.0f, 0.0f,  0.0f,  0.0f, 1.0f, -0.5f, -0.5f, -0.5f, -1.0f, 0.0f,  0.0f,  0.0f, 1.0f,
      -0.5f, -0.5f, 0.5f,  -1.0f, 0.0f,  0.0f,  0.0f, 0.0f, -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,  1.0f, 0.0f,

      0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.5f,  0.5f,  -0.5f, 1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
      0.5f,  -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,  0.0f, 1.0f, 0.5f,  -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
      0.5f,  -0.5f, 0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f, 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

      -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  0.0f, 1.0f, 0.5f,  -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  1.0f, 1.0f,
      0.5f,  -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,  1.0f, 0.0f, 0.5f,  -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,  1.0f, 0.0f,
      -0.5f, -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,  0.0f, 0.0f, -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  0.0f, 1.0f,

      -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.5f,  0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
      0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f, 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
      -0.5f, 0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f, -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,  0.0f, 1.0f};
}
// clang-format on
std::vector<GLuint> Yeager::GenerateCubeIndices()
{
  return std::vector<GLuint>{0, 1, 3, 3, 1, 2, 1, 5, 2, 2, 5, 6, 5, 4, 6, 6, 4, 7,
                             4, 0, 7, 7, 0, 3, 3, 2, 7, 7, 2, 6, 4, 5, 0, 0, 5, 1};
}

std::vector<GLfloat> Yeager::GenerateSphereVertices(int stackCount, int sectorCount)
{
  std::vector<GLfloat> vertices;
  int radius = 10;

  float x, y, z, xy;
  float nx, ny, nz, lenghtInv = 1.0f / radius;
  float s, t;

  float sectorStep = 2 * PI / sectorCount;
  float stackStep = PI / stackCount;
  float sectorAngle, stackAngle;

  for (int i = 0; i <= stackCount; ++i) {
    stackAngle = PI / 2 - i * stackStep;
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
