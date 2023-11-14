#include "ImportedObj.h"
#include "../../Application.h"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<MeshTexture> textures)
{
  m_vertices = vertices;
  m_indices = indices;
  m_textures = textures;
  Setup();
}

void Mesh::Setup()
{
  glGenVertexArrays(1, &m_vao);
  glGenBuffers(1, &m_vbo);
  glGenBuffers(1, &m_ebo);

  glBindVertexArray(m_vao);
  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

  glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), &m_vertices[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), &m_indices[0], GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_normal));

  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_texCoords));

  glBindVertexArray(0);
}

void Mesh::Draw(Yeager::Shader* shader)
{

  shader->UseShader();
  unsigned int diffuseNum = 1;
  unsigned int specularNum = 1;

  for (unsigned int x = 0; x < m_textures.size(); x++) {
    glActiveTexture(GL_TEXTURE0 + x);
    YgString number;
    YgString name = m_textures[x].m_name;
    if (name == "texture_diffuse") {
      number = std::to_string(diffuseNum++);
    } else if (name == "texture_specular") {
      number = std::to_string(specularNum++);
    }
    shader->SetInt((name + number).c_str(), x);
    glBindTexture(GL_TEXTURE_2D, m_textures[x].m_id);
  }

  glBindVertexArray(m_vao);
  glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(m_indices.size()), GL_UNSIGNED_INT, 0);

  glBindVertexArray(0);
  glActiveTexture(GL_TEXTURE0);
}

ImportedObject::ImportedObject(YgString path, Yeager::ApplicationCore* app, YgString name, bool flip)
    : m_app(app), m_model_path(path), Yeager::GameEntity(name), m_physics(Yeager::EntityPhysics(this)), m_flip(flip)
{

  LoadModel(path);
  auto toolbox = std::make_shared<Yeager::ToolBoxObject>();
  toolbox->SetType(ExplorerObjectType::kImportedObject);
  toolbox->SetTransformation(this);
  toolbox->SetPhysics(&m_physics);
  m_app->GetScene()->GetToolboxs()->push_back(toolbox);
}

void ImportedObject::ProcessTransformation(Yeager::Shader* shader)
{
  m_transformation.model = glm::translate(m_transformation.model, m_transformation.position);
  m_transformation.model =
      glm::rotate(m_transformation.model, glm::radians(m_transformation.rotation.x), YgVector3(1.0f, 0.0f, 0.0f));
  m_transformation.model =
      glm::rotate(m_transformation.model, glm::radians(m_transformation.rotation.y), YgVector3(0.0f, 1.0f, 0.0f));
  m_transformation.model =
      glm::rotate(m_transformation.model, glm::radians(m_transformation.rotation.z), YgVector3(0.0f, 0.0f, 1.0f));
  m_transformation.model = glm::scale(m_transformation.model, m_transformation.scale);

  shader->SetMat4("model", m_transformation.model);
  m_transformation.model = YgMatrix4(1.0f);
}
void ImportedObject::Draw(Yeager::Shader* shader)
{
  ProcessTransformation(shader);
  m_physics.ApplyGravity();
  for (unsigned int x = 0; x < meshes.size(); x++) {
    meshes[x].Draw(shader);
  }
}

ImportedObject::~ImportedObject()
{
  for (unsigned int x = 0; x < meshes.size(); x++) {
    glDeleteBuffers(1, meshes[x].GetEbo());
    glDeleteBuffers(1, meshes[x].GetVbo());
    glDeleteVertexArrays(1, meshes[x].GetVao());
  }
}

void ImportedObject::LoadModel(YgString path)
{
  Assimp::Importer imp;
  const aiScene* scene = imp.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
    m_app->GetInterface()->AddWarningWindow("Cannot load imported model! Path: " + path);
    Yeager::Log(ERROR, "Cannot load imported model! Path: {}, Error: {}", path.c_str(), imp.GetErrorString());
    return;
  }
  ProcessNode(scene->mRootNode, scene);
  Yeager::Log(INFO, "Success in loading imported model: {}", path.c_str());
  Yeager::Log(INFO, "Model information: Vertices {}, Indices {}, Textures loaded: {}", m_num_vertices, m_num_indices,
              m_textures_loaded.size());
}

void ImportedObject::ProcessNode(aiNode* node, const aiScene* scene)
{
  for (unsigned int x = 0; x < node->mNumMeshes; x++) {
    aiMesh* mesh = scene->mMeshes[node->mMeshes[x]];

    meshes.push_back(ProcessMesh(mesh, scene));
  }
  for (unsigned int x = 0; x < node->mNumChildren; x++) {
    ProcessNode(node->mChildren[x], scene);
  }
}

Mesh ImportedObject::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;
  std::vector<MeshTexture> textures;

  m_num_vertices += mesh->mNumVertices;

  for (unsigned int x = 0; x < mesh->mNumVertices; x++) {
    Vertex vertex;

    YgVector3 vector;
    vector.x = mesh->mVertices[x].x;
    vector.y = mesh->mVertices[x].y;
    vector.z = mesh->mVertices[x].z;
    vertex.m_position = vector;

    vector.x = mesh->mNormals[x].x;
    vector.y = mesh->mNormals[x].y;
    vector.z = mesh->mNormals[x].z;
    vertex.m_normal = vector;

    if (mesh->mTextureCoords[0]) {  // has texture coords
      YgVector2 vec;
      vec.x = mesh->mTextureCoords[0][x].x;
      vec.y = mesh->mTextureCoords[0][x].y;
      vertex.m_texCoords = vec;
    } else {  // No texture coords
      vertex.m_texCoords = YgVector2(0.0f, 0.0f);
    }
    vertices.push_back(vertex);
  }

  for (unsigned int x = 0; x < mesh->mNumFaces; x++) {
    aiFace face = mesh->mFaces[x];
    m_num_indices += face.mNumIndices;
    for (unsigned int y = 0; y < face.mNumIndices; y++) {
      indices.push_back(face.mIndices[y]);
    }
  }

  if (mesh->mMaterialIndex >= 0) {
    aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];
    std::vector<MeshTexture> diffuseMap = LoadMaterialTexture(mat, aiTextureType_DIFFUSE, "texture_diffuse");
    textures.insert(textures.end(), diffuseMap.begin(), diffuseMap.end());
    std::vector<MeshTexture> specularMap = LoadMaterialTexture(mat, aiTextureType_SPECULAR, "texture_specular");
    textures.insert(textures.end(), specularMap.begin(), specularMap.end());
  }
  return Mesh(vertices, indices, textures);
}

std::vector<MeshTexture> ImportedObject::LoadMaterialTexture(aiMaterial* mat, aiTextureType type, YgString typeName)
{
  std::vector<MeshTexture> textures;
  for (unsigned int x = 0; x < mat->GetTextureCount(type); x++) {
    aiString str;
    mat->GetTexture(type, x, &str);
    bool skip = false;
    for (unsigned int y = 0; y < m_textures_loaded.size(); y++) {
      YgString cmp_path = RemoveSuffixUntilCharacter(m_model_path, YG_PS);
      cmp_path += str.C_Str();
      if (std::strcmp(m_textures_loaded[y].m_path.data(), cmp_path.data()) == 0) {
        textures.push_back(m_textures_loaded[y]);
        skip = true;
        break;
      }
    }

    if (!skip) {
      YgString path_suffix_removed = RemoveSuffixUntilCharacter(m_model_path, YG_PS);
      YgString path = path_suffix_removed + str.C_Str();

      MeshTexture tex;
      tex.m_id = LoadTextureFromFile(path, m_flip);
      tex.m_name = typeName;
      tex.m_path = path;
      textures.push_back(tex);
      m_textures_loaded.push_back(tex);
    }
  }

  return textures;
}