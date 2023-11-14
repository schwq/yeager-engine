//    Yeager Engine, free and open source 3D/2D renderer written in OpenGL
//    In case of questions and bugs, please, refer to the issue tab on github
//    Repo : https://github.com/schwq/yeager-engine
//    Copyright (C) 2023
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include "../../Common/Common.h"
#include "../Editor/ToolboxObj.h"
#include "../Physics/PhysicsHandle.h"
#include "Entity.h"
#include "TextureHandle.h"

#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>

namespace Yeager {
class Texture2D;
class ToolBoxObject;
class ApplicationCore;
}  // namespace Yeager

struct Vertex {
  YgVector3 m_position;
  YgVector3 m_normal;
  YgVector2 m_texCoords;
};

struct MeshTexture {
  YgString m_name;
  unsigned int m_id;
  YgString m_path;
};

class Mesh {
 public:
  Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<MeshTexture> textures);

  void Draw(Yeager::Shader* shader);

  constexpr unsigned int* GetVao() { return &m_vao; }
  constexpr unsigned int* GetVbo() { return &m_vbo; }
  constexpr unsigned int* GetEbo() { return &m_ebo; }
  std::vector<MeshTexture>* GetTextures() { return &m_textures; }

 private:
  unsigned int m_vao, m_vbo, m_ebo;
  std::vector<Vertex> m_vertices;
  std::vector<unsigned int> m_indices;
  std::vector<MeshTexture> m_textures;
  void Setup();
};

class ImportedObject : public Yeager::GameEntity {
 public:
  ImportedObject(YgString path, Yeager::ApplicationCore* app, YgString name = "DEFAULT", bool flip = true);
  ~ImportedObject();
  void Draw(Yeager::Shader* shader);
  void operator=(const ImportedObject& other);
  YgString GetPath() { return m_model_path; }
  std::vector<MeshTexture>* GetTexturesLoaded() { return &m_textures_loaded; }

 private:
  void SetupVao();
  void LoadModel(YgString path);
  void ProcessNode(aiNode* node, const aiScene* scene);
  Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
  void ProcessTransformation(Yeager::Shader* shader);
  std::vector<MeshTexture> LoadMaterialTexture(aiMaterial* mat, aiTextureType type, YgString typeName);
  Yeager::ApplicationCore* m_app;
  std::vector<MeshTexture> m_textures_loaded;
  std::vector<Mesh> meshes;
  YgString m_model_path;
  Yeager::EntityPhysics m_physics;
  bool m_flip;

  unsigned int m_num_vertices = 0;
  unsigned int m_num_indices = 0;
};
