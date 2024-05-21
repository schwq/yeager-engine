//    Yeager Engine, free and open source 3D/2D renderer written in OpenGL
//    In case of questions and bugs, please, refer to the issue tab on github
//    Repo : https://github.com/schwq/YeagerEngine
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

#include "../../Application.h"
#include "../../Common/Common.h"
#include "../../Common/LogEngine.h"
#include "../../Common/Utilities.h"
#include "ImageUtilities.h"
#include "Importer.h"
#include "Object.h"
#include "ShaderHandle.h"
namespace Yeager {

enum class SkyboxTextureType { ESamplerCube, ESampler2D };

class Skybox : public EditorEntity {

 public:
  Skybox(String name, ObjectGeometryType::Enum type, ApplicationCore* application);
  ~Skybox();

  bool BuildSkyboxFromCubemap(String directory, Yeager::ImageExtension ext, bool flip = false);
  bool BuildSkyboxFrom2DTexture(String path, bool flip = false);
  bool BuildSkyboxFromImport(String path, bool flip = false);
  void Draw(Yeager::Shader* shader, Matrix4 view, Matrix4 projection);

  String GetPath() const { return Path; }

  SkyboxTextureType GetTextureType() const { return m_Type; }

  ObjectGeometryData* GetGeometryData() { return &m_Data; }
  ObjectModelData* GetModelData() { return &m_Model; }

  ObjectGeometryType::Enum GetGeometry() const { return m_Geometry; }

 private:
  void Setup();
  void SetupModel();
  void GenerateGeometry();
  void GenerateCubemapGeometry();

  bool m_SkyboxDataLoaded = false;

  std::shared_ptr<MaterialTexture2D> m_Texture = YEAGER_NULLPTR;

  Uint m_VerticesIndex = 0;
  GLuint m_Vao = -1, m_Vbo = -1, m_Ebo = -1;
  String Path = YEAGER_NULL_LITERAL;

  SkyboxTextureType m_Type;
  ObjectGeometryData m_Data;
  ObjectModelData m_Model;
  ObjectGeometryType::Enum m_Geometry;
  std::shared_ptr<ToolboxHandle> m_Toolbox = YEAGER_NULLPTR;
};

static std::vector<GLfloat> GenerateSkyboxVertices()
{
  // positions
  return std::vector<GLfloat>{-1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f,
                              1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f,

                              -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f,
                              -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,

                              1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,
                              1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f,

                              -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
                              1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,

                              -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,
                              1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f,

                              -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f,
                              1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f};
};  // namespace Yeager

}  // namespace Yeager