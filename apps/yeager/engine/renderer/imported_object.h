#pragma once

#include "../../application.h"
#include "../../common/common.h"
#include "../editor/toolbox_object.h"
#include "renderer_objects.h"
#include "texture.h"

#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>

class EngineTexture2D;

struct Vertex {
  Vector3 m_position;
  Vector3 m_normal;
  Vector2 m_texCoords;
};

struct MeshTexture {
  String m_name;
  uint m_id;
  String m_path;
};

class Mesh {
 public:
  Mesh(std::vector<Vertex> vertices, std::vector<uint> indices,
       std::vector<MeshTexture> textures);

  void Draw(Shader* shader);

  constexpr uint* GetVao() { return &m_vao; }
  constexpr uint* GetVbo() { return &m_vbo; }
  constexpr uint* GetEbo() { return &m_ebo; }

 private:
  uint m_vao, m_vbo, m_ebo;
  std::vector<Vertex> m_vertices;
  std::vector<uint> m_indices;
  std::vector<MeshTexture> m_textures;
  void Setup();
};

class ImportedObject {
 public:
  ImportedObject(String path, Application* app, String name = "DEFAULT");
  ~ImportedObject();
  void Draw(Shader* shader);

 private:
  void SetupVao();
  void LoadModel(String path);
  void ProcessNode(aiNode* node, const aiScene* scene);
  Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
  void ProcessTransformation(Shader* shader);
  std::vector<MeshTexture> LoadMaterialTexture(aiMaterial* mat,
                                               aiTextureType type,
                                               String typeName);
  ObjectSpaceProp m_space_prop;
  Application* m_app;
  std::vector<MeshTexture> m_textures_loaded;
  std::vector<Mesh> meshes;
  String m_model_path;
  String m_name;
  ToolBoxObject* m_toolbox_obj;
};