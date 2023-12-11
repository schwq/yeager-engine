#include "Object.h"
#include "../../Application.h"
#include "Importer.h"
using namespace Yeager;

YgString Yeager::ObjectGeometryTypeToString(ObjectGeometryType type)
{
  switch (type) {
    case ObjectGeometryType::ECube:
      return "Cube";
    case ObjectGeometryType::ESphere:
      return "Sphere";
    case ObjectGeometryType::ETriangule:
      return "Triangle";
    case ObjectGeometryType::ECustom:
      return "Custom";
    default:
      return "ERROR_CANNOT_FIND_TYPE";
  }
}

ObjectGeometryType Yeager::StringToObjectGeometryType(const YgString& str)
{
  switch (Yeager::StringToInteger(str.c_str())) {
    case Yeager::StringToInteger("Cube"):
      return ObjectGeometryType::ECube;
    case Yeager::StringToInteger("Sphere"):
      return ObjectGeometryType::ESphere;
    case Yeager::StringToInteger("Triangle"):
      return ObjectGeometryType::ETriangule;
    case Yeager::StringToInteger("Custom"):
      return ObjectGeometryType::ECustom;
    default:
      Yeager::Log(ERROR,
                  "StringToObjectGeometryType error! Cannot find the type of the string given! Returning Custom!");
      return ObjectGeometryType::ECustom;
  }
}

Object::Object(YgString name, ApplicationCore* application)
    : GameEntity(name), m_Application(application), m_Physics(this)
{
  m_Toolbox = std::make_shared<ToolBoxObject>();
}

Object::~Object()
{
  if (m_ObjectDataLoaded) {
    if (m_GeometryType == ObjectGeometryType::ECustom) {
      glDeleteVertexArrays(1, &m_GeometryData.m_Vao);
      glDeleteBuffers(1, &m_GeometryData.m_Vbo);
      glDeleteBuffers(1, &m_GeometryData.m_Ebo);
    } else {
      for (auto& mesh : m_ModelData.Meshes) {
        DeleteMeshGLBuffers(&mesh);
      }
    }
    Yeager::Log(INFO, "Destroying object {}", m_Name);
  }
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

bool Yeager::DrawSeparateMesh(ObjectMeshData* mesh, Yeager::Shader* shader)
{

  unsigned int diffuseNum = 1;
  unsigned int specularNum = 1;

  for (unsigned int x = 0; x < mesh->Textures.size(); x++) {
    glActiveTexture(GL_TEXTURE0 + x);
    YgString number;
    YgString name = mesh->Textures[x].Name;
    if (name == "texture_diffuse") {
      number = std::to_string(diffuseNum++);
    } else if (name == "texture_specular") {
      number = std::to_string(specularNum++);
    }
    shader->SetInt((name + number).c_str(), x);
    glBindTexture(GL_TEXTURE_2D, mesh->Textures[x].ID);
  }

  glBindVertexArray(mesh->m_Vao);
  glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh->Indices.size()), GL_UNSIGNED_INT, nullptr);

  glBindVertexArray(0);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, 0);
}

bool Yeager::DrawSeparateInstancedMesh(ObjectMeshData* mesh, Yeager::Shader* shader, int amount)
{
  unsigned int diffuseNum = 1;
  unsigned int specularNum = 1;

  for (unsigned int x = 0; x < mesh->Textures.size(); x++) {
    glActiveTexture(GL_TEXTURE0 + x);
    YgString number;
    YgString name = mesh->Textures[x].Name;
    if (name == "texture_diffuse") {
      number = std::to_string(diffuseNum++);
    } else if (name == "texture_specular") {
      number = std::to_string(specularNum++);
    }
    shader->SetInt((name + number).c_str(), x);
    glBindTexture(GL_TEXTURE_2D, mesh->Textures[x].ID);
  }

  glBindVertexArray(mesh->m_Vao);
  glDrawElementsInstanced(GL_TRIANGLES, static_cast<GLsizei>(mesh->Indices.size()), GL_UNSIGNED_INT, nullptr, amount);

  glBindVertexArray(0);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, 0);
}

bool Object::GenerateObjectGeometry(ObjectGeometryType geometry)
{
  if (!m_ObjectDataLoaded) {
    m_GeometryType = geometry;
    switch (geometry) {
      case ObjectGeometryType::ECube:
        m_GeometryData.Vertices = GenerateCubeVertices();
        m_GeometryData.Indices = GenerateCubeIndices();
        Setup();
        m_ObjectDataLoaded = true;

        Yeager::Log(INFO, "Success in loading geometry {}", m_Name);

        m_Toolbox->SetTransformation(this);
        m_Toolbox->SetPhysics(GetPhysics());
        m_Toolbox->SetType(ExplorerObjectType::kShapes);
        m_Application->GetScene()->GetToolboxs()->push_back(m_Toolbox);

        return true;
      default:
        Yeager::Log(ERROR, "Cannot generate geometry, invalid type! model {}", m_Name);
        return false;
    }
  } else {
    Yeager::Log(WARNING, "Object data already loaded! Trying to load geometry to model {}", m_Name);
    return false;
  }
}

bool Object::ImportObjectFromFile(YgCchar path, bool flip_image)
{
  m_Path = path;

  if (!m_ObjectDataLoaded) {
    Importer imp(m_Name);
    m_ModelData = imp.Import(path, flip_image);

    if (!m_ModelData.SuccessfulLoaded) {
      Yeager::Log(ERROR, "Cannot load imported model data, model {} path {}", m_Name, path);
      m_ObjectDataLoaded = false;
      return false;
    }
    m_GeometryType = ObjectGeometryType::ECustom;
    m_ObjectDataLoaded = true;
    Setup();

    m_Toolbox->SetTransformation(this);
    m_Toolbox->SetPhysics(GetPhysics());
    m_Toolbox->SetType(ExplorerObjectType::kImportedObject);
    m_Application->GetScene()->GetToolboxs()->push_back(m_Toolbox);

    for (auto& tex : m_ModelData.TexturesLoaded) {
      m_EntityLoadedTextures.push_back(&tex);
    }

    Yeager::Log(INFO, "Success in loading mode {} path {}", m_Name, path);
    return true;
  } else {
    Yeager::Log(WARNING, "Model data already loaded! Trying to load imported model to model {} path {}", m_Name, path);
    return false;
  }
}

void InstancedObject::DrawGeometry(Yeager::Shader* shader)
{
  glBindVertexArray(m_GeometryData.m_Vao);

  glDrawElementsInstanced(GL_TRIANGLES, static_cast<unsigned int>(m_GeometryData.Indices.size()), GL_UNSIGNED_INT, 0,
                          m_InstancedObjs);

  glBindVertexArray(0);
  glActiveTexture(GL_TEXTURE0);
}

void Object::DrawGeometry(Yeager::Shader* shader)
{
  glBindVertexArray(m_GeometryData.m_Vao);

  glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(m_GeometryData.Indices.size()), GL_UNSIGNED_INT, 0);

  glBindVertexArray(0);
  glActiveTexture(GL_TEXTURE0);
}

void Object::DrawModel(Yeager::Shader* shader)
{
  for (auto& mesh : m_ModelData.Meshes) {
    DrawSeparateMesh(&mesh, shader);
  }
}

void InstancedObject::DrawModel(Yeager::Shader* shader, int amount)
{
  for (auto& mesh : m_ModelData.Meshes) {
    DrawSeparateInstancedMesh(&mesh, shader, amount);
  }
}

void Object::Draw(Yeager::Shader* shader)
{
  if (m_ObjectDataLoaded && m_Render) {
    shader->UseShader();
    ProcessTransformation(shader);
    if (m_GeometryType == ObjectGeometryType::ECustom) {
      DrawModel(shader);
    } else {
      DrawGeometry(shader);
    }
  }
}

void InstancedObject::BuildProp(const std::vector<YgVector3>& positions, Shader* shader)
{
  shader->UseShader();
  for (unsigned int x = 0; x < m_InstancedObjs; x++) {
    shader->SetVec3("offsets[" + std::to_string(x) + "]", positions[x]);
  }
}

void InstancedObject::Draw(Yeager::Shader* shader, int amount)
{
  if (m_ObjectDataLoaded && m_Render) {
    shader->UseShader();
    ProcessTransformation(shader);
    if (m_GeometryType == ObjectGeometryType::ECustom) {
      DrawModel(shader, m_InstancedObjs);
    } else {
      DrawGeometry(shader);
    }
  }
}

void Object::Setup()
{
  if (m_GeometryType == ObjectGeometryType::ECustom) {
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
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_GeometryData.Indices.size() * sizeof(uint), &m_GeometryData.Indices[0],
                 GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
  }
}

AnimatedObject::AnimatedObject(YgString name, ApplicationCore* application) : Object(name, application) {}
bool AnimatedObject::ImportObjectFromFile(YgCchar path, bool flip_image)
{
  m_Path = path;

  if (!m_ObjectDataLoaded) {
    Importer imp(m_Name);
    m_ModelData = imp.ImportAnimated(path, flip_image);

    if (!m_ModelData.SuccessfulLoaded) {
      Yeager::Log(ERROR, "Cannot load imported model data, model {} path {}", m_Name, path);
      m_ObjectDataLoaded = false;
      return false;
    }
    m_GeometryType = ObjectGeometryType::ECustom;
    m_ObjectDataLoaded = true;
    AnimatedObject::Setup();

    m_Toolbox->SetTransformation(this);
    m_Toolbox->SetPhysics(GetPhysics());
    m_Toolbox->SetType(ExplorerObjectType::kImportedObject);
    m_Application->GetScene()->GetToolboxs()->push_back(m_Toolbox);

    for (auto& tex : m_ModelData.TexturesLoaded) {
      m_EntityLoadedTextures.push_back(&tex);
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
    ProcessTransformation(shader);
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
      YgString number;
      YgString name = mesh.Textures[x].Name;
      if (name == "texture_diffuse") {
        number = std::to_string(diffuseNum++);
      } else if (name == "texture_specular") {
        number = std::to_string(specularNum++);
      } else if (name == "texture_normal") {
        number = std::to_string(normalNum++);
      } else if (name == "texture_height") {
        number = std::to_string(heightNum++);
      }
      shader->SetInt((name + number).c_str(), x);
      glBindTexture(GL_TEXTURE_2D, mesh.Textures[x].ID);
    }

    glBindVertexArray(mesh.m_Vao);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh.Indices.size()), GL_UNSIGNED_INT, nullptr);

    glBindVertexArray(0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
  }
}

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
std::vector<GLuint> Yeager::GenerateCubeIndices()
{
  return std::vector<GLuint>{0, 1, 3, 3, 1, 2, 1, 5, 2, 2, 5, 6, 5, 4, 6, 6, 4, 7,
                             4, 0, 7, 7, 0, 3, 3, 2, 7, 7, 2, 6, 4, 5, 0, 0, 5, 1};
}
