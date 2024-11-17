//    Yeager Engine, free and open source 3D/2D renderer written in OpenGL
//    In case of questions and bugs, please, refer to the issue tab on github
//    Repo : https://github.com/schwq/YeagerEngine
//    Copyright (C) 2023 - Present
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
#include "OpenGLRender.h"
#include "ShaderHandle.h"

namespace Yeager {

#define YEAGER_SKYBOX_DEFAULT_NAME "Scene Skybox"

enum class SkyboxTextureType { ESamplerCube, ESampler2D };

class Skybox : public EditorEntity {

 public:
  Skybox(const EntityBuilder& builder, ObjectGeometryType::Enum type);
  ~Skybox();

  bool BuildSkyboxFromCubemap(String directory, Yeager::ImageExtension ext, bool flip = false);
  bool BuildSkyboxFrom2DTexture(String path, bool flip = false);
  bool BuildSkyboxFromImport(String path, bool flip = false);
  void Draw(Yeager::Shader* shader, Matrix4 view, Matrix4 projection);

  YEAGER_NODISCARD String GetPath() const { return Path; }
  YEAGER_NODISCARD SkyboxTextureType GetTextureType() const { return m_Type; }
  YEAGER_NODISCARD ObjectGeometryData* GetGeometryData() { return &m_Data; }
  YEAGER_NODISCARD ObjectModelData* GetModelData() { return &m_Model; }
  YEAGER_NODISCARD ObjectGeometryType::Enum GetGeometry() const { return m_Geometry; }
  YEAGER_NODISCARD bool IsLoaded() const { return m_SkyboxDataLoaded; }

 private:
  void Setup();
  void SetupModel();
  void GenerateGeometry();
  void GenerateCubemapGeometry();

  bool m_SkyboxDataLoaded = false;

  std::shared_ptr<MaterialTexture2D> m_Texture = YEAGER_NULLPTR;

  Uint m_VerticesIndex = 0;
  String Path = YEAGER_NULL_LITERAL;

  SimpleRenderer m_Renderer;

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