
#include "Skybox.h"

#include "Object.h"
#include "TextureHandle.h"
using namespace Yeager;

Skybox::Skybox(String name, ObjectGeometryType::Enum type, ApplicationCore* application)
    : EditorEntity(EntityObjectType::SKYBOX, application, name), m_Geometry(type)
{
  Yeager::Log(INFO, "Loading skybox {}", name);
  m_Texture = std::make_shared<MaterialTexture2D>(application, name, MaterialTextureType::eDIFFUSE);
}

bool Skybox::BuildSkyboxFromImport(String path, bool flip)
{
  if (!m_SkyboxDataLoaded) {
    Path = path;

    Importer imp("Skybox", m_Application);
    m_Model = imp.Import(path.c_str(), flip);

    if (!m_Model.SuccessfulLoaded) {
      Yeager::Log(ERROR, "Cannot load model to skybox!");
      return false;
    }

    m_Texture->GetTextureDataHandle()->Texture = m_Model.TexturesLoaded[0]->first.GetTextureID();
    m_Geometry = ObjectGeometryType::eCUSTOM;
    m_Type = SkyboxTextureType::ESampler2D;
    m_SkyboxDataLoaded = true;
    SetupModel();
    return true;

  } else {
    Yeager::Log(WARNING, "Trying to build skybox that already have been bullt!");
    return false;
  }
}

void Skybox::GenerateGeometry()
{
  switch (m_Geometry) {
    case ObjectGeometryType::eCUBE:
      m_Data.Vertices = Yeager::GenerateCubeVertices();
      m_VerticesIndex = m_Data.Vertices.size() / 3;
      break;
    default:
      Yeager::Log(ERROR, "Skybox generate geometry, type not implemented");
  }
}

void Skybox::GenerateCubemapGeometry()
{
  m_Data.Vertices = GenerateSkyboxVertices();
  m_VerticesIndex = m_Data.Vertices.size() / 3;
}

bool Skybox::BuildSkyboxFromCubemap(String directory, Yeager::ImageExtension ext, bool flip)
{
  if (!m_SkyboxDataLoaded) {
    if (NumberOfFilesInDir(directory) == 6) {
      std::vector<String> paths = {
          (directory + "right" + ImageExtensionToString(ext)), (directory + "left" + ImageExtensionToString(ext)),
          (directory + "top" + ImageExtensionToString(ext)),   (directory + "bottom" + ImageExtensionToString(ext)),

          (directory + "front" + ImageExtensionToString(ext)), (directory + "back" + ImageExtensionToString(ext))};
      m_Texture->GenerateCubeMapFromFile(paths, flip);
      m_SkyboxDataLoaded = true;
      m_Type = SkyboxTextureType::ESamplerCube;
      GenerateCubemapGeometry();
      Setup();
      return true;
    } else if (NumberOfFilesInDir(directory) == 1) {
      std::vector<String> paths;
      for (unsigned int x = 0; x < 6; x++) {
        for (const auto& path : std::filesystem::directory_iterator(directory)) {
          paths.push_back(path.path().string());
        }
      }
      m_Texture->GenerateCubeMapFromFile(paths, flip);
      m_SkyboxDataLoaded = true;
      m_Type = SkyboxTextureType::ESamplerCube;
      GenerateCubemapGeometry();
      Setup();
      return true;
    } else {
      Yeager::Log(WARNING,
                  "Skybox images directory dont have the standard organization, it doesnt have 6 images nor 1 image to "
                  "complete convertion!");
      return false;
    }
  } else {
    Yeager::Log(WARNING, "Trying to build skybox that already have been bullt!");
    return false;
  }
}

bool Skybox::BuildSkyboxFrom2DTexture(String path, bool flip)
{
  if (!m_SkyboxDataLoaded) {
    m_Texture->GenerateFromFile(path, flip);
    m_SkyboxDataLoaded = true;
    m_Type = SkyboxTextureType::ESampler2D;
    GenerateGeometry();
    Setup();
    return true;
  } else {
    Yeager::Log(WARNING, "Trying to build skybox that already have been bullt!");
    return false;
  }
}

void Skybox::SetupModel()
{

  for (auto& mesh : m_Model.Meshes) {

    glGenVertexArrays(1, &mesh.m_Vao);
    glGenBuffers(1, &mesh.m_Vbo);
    glGenBuffers(1, &mesh.m_Ebo);

    glBindVertexArray(mesh.m_Vao);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.m_Vbo);

    glBufferData(GL_ARRAY_BUFFER, mesh.Vertices.size() * sizeof(ObjectVertexData), &mesh.Vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.m_Ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.Indices.size() * sizeof(unsigned int), &mesh.Indices[0], GL_STATIC_DRAW);

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
}

void Skybox::Setup()
{

  glGenVertexArrays(1, &m_Vao);
  glGenBuffers(1, &m_Vbo);

  glBindVertexArray(m_Vao);
  glBindBuffer(GL_ARRAY_BUFFER, m_Vbo);
  glBufferData(GL_ARRAY_BUFFER, m_Data.Vertices.size() * sizeof(GLfloat), &m_Data.Vertices[0], GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, (m_Type == SkyboxTextureType::ESamplerCube ? 3 : 8) * sizeof(float),
                        (void*)0);

  if (m_Type == SkyboxTextureType::ESampler2D) {

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(GLfloat) * 3));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
  }

  glBindVertexArray(0);
}

Skybox::~Skybox()
{
  if (m_SkyboxDataLoaded) {
    glDeleteVertexArrays(1, &m_Vao);
    glDeleteBuffers(1, &m_Vbo);
    glDeleteBuffers(1, &m_Ebo);
  }
}

void Skybox::Draw(Yeager::Shader* shader, Matrix4 view, Matrix4 projection)
{
  if (m_SkyboxDataLoaded && m_Render) {
    glDepthFunc(GL_LEQUAL);
    shader->UseShader();
    shader->SetMat4("view", view);
    shader->SetMat4("projection", projection);
    glBindVertexArray(m_Vao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(m_Type == SkyboxTextureType::ESampler2D ? GL_TEXTURE_2D : GL_TEXTURE_CUBE_MAP,
                  m_Texture->GetTextureID());
    if (m_Geometry != ObjectGeometryType::eCUSTOM) {
      glDrawArrays(GL_TRIANGLES, 0, m_VerticesIndex);
    } else {
      for (auto& mesh : m_Model.Meshes) {
        Yeager::DrawSeparateMesh(&mesh, shader);
      }
    }
    glBindVertexArray(0);
    glDepthFunc(GL_LESS);
  }
}