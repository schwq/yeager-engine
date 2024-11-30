
#include "Skybox.h"
#include "Components/Renderer/Objects/Object.h"
#include "Components/Renderer/Texture/TextureHandle.h"
using namespace Yeager;

Skybox::Skybox(const EntityBuilder& builder, ObjectGeometryType::Enum type)
    : EditorEntity(EntityBuilder(builder.Application, builder.Name, EntityObjectType::SKYBOX, builder.UUID)),
      m_Geometry(type)
{
  Yeager::Log(INFO, "Loading skybox {}", mName);
  m_Texture = BaseAllocator::MakeSharedPtr<MaterialTexture2D>(EntityBuilder(mApplication, mName),
                                                              MaterialTextureType::eDIFFUSE);
}

bool Skybox::BuildSkyboxFromImport(String path, bool flip)
{
  if (!m_SkyboxDataLoaded) {
    Path = path;

    Importer imp("Skybox", mApplication);
    ObjectCreationConfiguration configuration = ObjectCreationConfiguration();
    m_Model = imp.Import(path.c_str(), configuration, flip);

    if (!m_Model.SuccessfulLoaded) {
      Yeager::Log(ERROR, "Cannot load model to skybox!");
      return false;
    }

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

bool Skybox::BuildSkyboxFromCubemap(String directory, Yeager::ImageExtension::Enum ext, bool flip)
{
  if (!m_SkyboxDataLoaded) {
    if (NumberOfFilesInDir(directory) == 6) {
      String ex = ImageExtension::ImageExtensionToString(ext);
      std::vector<String> paths = {(directory + "right" + ex),  (directory + "left" + ex),  (directory + "top" + ex),
                                   (directory + "bottom" + ex), (directory + "front" + ex), (directory + "back" + ex)};
      m_Texture->GenerateCubeMapFromFile(paths, flip);
      m_SkyboxDataLoaded = true;
      m_Type = SkyboxTextureType::ESamplerCube;
      GenerateCubemapGeometry();
      Setup();
      return true;
    } else if (NumberOfFilesInDir(directory) == 1) {
      std::vector<String> paths;
      for (Uint x = 0; x < 6; x++) {
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
}

void Skybox::Setup()
{
  m_Renderer.GenBuffers();

  m_Renderer.BindBuffers();
  m_Renderer.BufferData(GL_ARRAY_BUFFER, m_Data.Vertices.size() * sizeof(GLfloat), &m_Data.Vertices[0], GL_STATIC_DRAW);

  m_Renderer.VertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                                 (m_Type == SkyboxTextureType::ESamplerCube ? 3 : 8) * sizeof(float), (void*)0);

  if (m_Type == SkyboxTextureType::ESampler2D) {

    m_Renderer.VertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(GLfloat) * 3));
    m_Renderer.VertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(GLfloat)));
  }

  m_Renderer.UnbindBuffers();
}

Skybox::~Skybox()
{
  if (m_SkyboxDataLoaded) {
    if (m_Geometry == ObjectGeometryType::eCUSTOM) {
      for (auto& mesh : m_Model.Meshes) {
        mesh.Renderer.DeleteBuffers();
      }
    } else {
      m_Renderer.DeleteBuffers();
    }
  }
}

void Skybox::Draw(Yeager::Shader* shader, Matrix4 view, Matrix4 projection)
{
  glDisable(GL_CULL_FACE);

  if (m_SkyboxDataLoaded && bRender) {
    glDepthFunc(GL_LEQUAL);
    shader->UseShader();
    shader->SetMat4("view", view);
    shader->SetMat4("projection", projection);
    m_Renderer.BindVertexArray();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(m_Type == SkyboxTextureType::ESampler2D ? GL_TEXTURE_2D : GL_TEXTURE_CUBE_MAP,
                  m_Model.TexturesLoaded[0]->first.GetTextureID());
    if (m_Geometry != ObjectGeometryType::eCUSTOM) {
      m_Renderer.Draw(GL_TRIANGLES, 0, m_VerticesIndex);
    } else {
      for (auto& mesh : m_Model.Meshes) {
        Yeager::DrawSeparateMesh(&mesh, shader);
      }
    }
    m_Renderer.UnbindVertexArray();
    glDepthFunc(GL_LESS);
  }

  glEnable(GL_CULL_FACE);
}