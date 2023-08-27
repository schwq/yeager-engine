#include "imported_object.h"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<uint> indices,
           std::vector<MeshTexture> textures) {
  m_vertices = vertices;
  m_indices = indices;
  m_textures = textures;
  Setup();
}

void Mesh::Setup() {
  glGenVertexArrays(1, &m_vao);
  glGenBuffers(1, &m_vbo);
  glGenBuffers(1, &m_ebo);

  glBindVertexArray(m_vao);
  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

  glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex),
               &m_vertices[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(uint),
               &m_indices[0], GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void*)offsetof(Vertex, m_normal));

  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void*)offsetof(Vertex, m_texCoords));

  glBindVertexArray(0);
}

void Mesh::Draw(Shader* shader) {

  shader->UseShader();
  uint diffuseNum = 1;
  uint specularNum = 1;

  for (uint x = 0; x < m_textures.size(); x++) {
    glActiveTexture(GL_TEXTURE0 + x);
    String number;
    String name = m_textures[x].m_name;
    if (name == "texture_diffuse") {
      number = std::to_string(diffuseNum++);
    } else if (name == "texture_specular") {
      number = std::to_string(specularNum++);
    }
    shader->SetInt((name + number).c_str(), x);
    glBindTexture(GL_TEXTURE_2D, m_textures[x].m_id);
  }

  glBindVertexArray(m_vao);
  glDrawElements(GL_TRIANGLES, static_cast<uint>(m_indices.size()),
                 GL_UNSIGNED_INT, 0);

  glBindVertexArray(0);
  glActiveTexture(GL_TEXTURE0);
}

ImportedObject::ImportedObject(String path, Application* app, String name)
    : m_app(app), m_model_path(path), m_name(name) {

  LoadModel(path);
  m_app->GetImportedObjects()->push_back(this);

  m_toolbox_obj = new ToolBoxObject();
  m_toolbox_obj->SetType(ExplorerObjectType::kImportedObject);
  m_toolbox_obj->SetPosition(&m_space_prop.m_position);
  m_toolbox_obj->SetRotation(&m_space_prop.m_rotation);
  m_toolbox_obj->SetScale(&m_space_prop.m_scaling);

  m_app->GetExplorer()->AddObject(
      name.c_str(), ExplorerObjectType::kImportedObject, m_toolbox_obj);
}

void ImportedObject::ProcessTransformation(Shader* shader) {
  m_space_prop.m_model =
      glm::translate(m_space_prop.m_model, m_space_prop.m_position);
  m_space_prop.m_model =
      glm::rotate(m_space_prop.m_model, glm::radians(m_space_prop.m_rotation.x),
                  Vector3(1.0f, 0.0f, 0.0f));
  m_space_prop.m_model =
      glm::rotate(m_space_prop.m_model, glm::radians(m_space_prop.m_rotation.y),
                  Vector3(0.0f, 1.0f, 0.0f));
  m_space_prop.m_model =
      glm::rotate(m_space_prop.m_model, glm::radians(m_space_prop.m_rotation.z),
                  Vector3(0.0f, 0.0f, 1.0f));
  m_space_prop.m_model =
      glm::scale(m_space_prop.m_model, m_space_prop.m_scaling);

  shader->SetMat4("model", m_space_prop.m_model);
  m_space_prop.m_model = Matrix4(1.0f);
}

void ImportedObject::Draw(Shader* shader) {
  ProcessTransformation(shader);
  for (uint x = 0; x < meshes.size(); x++) {
    meshes[x].Draw(shader);
  }
}

ImportedObject::~ImportedObject() {
  for (uint x = 0; x < meshes.size(); x++) {
    glDeleteBuffers(1, meshes[x].GetEbo());
    glDeleteBuffers(1, meshes[x].GetVbo());
    glDeleteVertexArrays(1, meshes[x].GetVao());
  }
}

void ImportedObject::LoadModel(String path) {
  Assimp::Importer imp;
  const aiScene* scene =
      imp.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
      !scene->mRootNode) {
    m_app->GetInterface()->AddWarningWindow(
        "Cannot load imported model! Path: " + path);
    VLOG_F(ERROR, "Cannot load imported model! Path: %s, Error: %s",
           path.c_str(), imp.GetErrorString());
    return;
  }
  ProcessNode(scene->mRootNode, scene);
  VLOG_F(INFO, "Success in loading imported model: %s", path.c_str());
}

void ImportedObject::ProcessNode(aiNode* node, const aiScene* scene) {
  for (uint x = 0; x < node->mNumMeshes; x++) {
    aiMesh* mesh = scene->mMeshes[node->mMeshes[x]];

    meshes.push_back(ProcessMesh(mesh, scene));
  }
  for (uint x = 0; x < node->mNumChildren; x++) {
    ProcessNode(node->mChildren[x], scene);
  }
}

Mesh ImportedObject::ProcessMesh(aiMesh* mesh, const aiScene* scene) {
  std::vector<Vertex> vertices;
  std::vector<uint> indices;
  std::vector<MeshTexture> textures;

  for (uint x = 0; x < mesh->mNumVertices; x++) {
    Vertex vertex;

    Vector3 vector;
    vector.x = mesh->mVertices[x].x;
    vector.y = mesh->mVertices[x].y;
    vector.z = mesh->mVertices[x].z;
    vertex.m_position = vector;

    vector.x = mesh->mNormals[x].x;
    vector.y = mesh->mNormals[x].y;
    vector.z = mesh->mNormals[x].z;
    vertex.m_normal = vector;

    if (mesh->mTextureCoords[0]) {  // has texture coords
      Vector2 vec;
      vec.x = mesh->mTextureCoords[0][x].x;
      vec.y = mesh->mTextureCoords[0][x].y;
      vertex.m_texCoords = vec;
    } else {  // No texture coords
      vertex.m_texCoords = Vector2(0.0f, 0.0f);
    }
    vertices.push_back(vertex);
  }

  for (uint x = 0; x < mesh->mNumFaces; x++) {
    aiFace face = mesh->mFaces[x];
    for (uint y = 0; y < face.mNumIndices; y++) {
      indices.push_back(face.mIndices[y]);
    }
  }

  if (mesh->mMaterialIndex >= 0) {
    aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];
    std::vector<MeshTexture> diffuseMap =
        LoadMaterialTexture(mat, aiTextureType_DIFFUSE, "texture_diffuse");
    textures.insert(textures.end(), diffuseMap.begin(), diffuseMap.end());
    std::vector<MeshTexture> specularMap =
        LoadMaterialTexture(mat, aiTextureType_SPECULAR, "texture_specular");
    textures.insert(textures.end(), specularMap.begin(), specularMap.end());
  }
  return Mesh(vertices, indices, textures);
}

std::vector<MeshTexture> ImportedObject::LoadMaterialTexture(aiMaterial* mat,
                                                             aiTextureType type,
                                                             String typeName) {
  std::vector<MeshTexture> textures;
  for (uint x = 0; x < mat->GetTextureCount(type); x++) {
    aiString str;
    mat->GetTexture(type, x, &str);
    bool skip = false;
    for (uint y = 0; y < m_textures_loaded.size(); y++) {
      String cmp_path = RemoveSuffixUntilCharacter(m_model_path, kPS);
      cmp_path += str.C_Str();
      if (std::strcmp(m_textures_loaded[y].m_path.data(), cmp_path.data()) ==
          0) {
        textures.push_back(m_textures_loaded[y]);
        skip = true;
        break;
      }
    }

    if (!skip) {
      String path_suffix_removed =
          RemoveSuffixUntilCharacter(m_model_path, kPS);
      String path = path_suffix_removed + str.C_Str();

      MeshTexture tex;
      tex.m_id = LoadTextureFromFile(path);
      tex.m_name = typeName;
      tex.m_path = path;
      textures.push_back(tex);
      m_textures_loaded.push_back(tex);
    }
  }

  return textures;
}