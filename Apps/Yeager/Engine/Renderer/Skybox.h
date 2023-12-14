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
#include "../../Common/LogEngine.h"
#include "../../Common/Utilities.h"
#include "ImageUtilities.h"
#include "Importer.h"
#include "Object.h"
#include "ShaderHandle.h"
#include "../../Application.h"
namespace Yeager {

enum class SkyboxTextureType { ESamplerCube, ESampler2D };

class Skybox : public Entity {

 public:
  Skybox(YgString name, ObjectGeometryType type, ApplicationCore* application, bool flip_image = false);
  ~Skybox();

  bool BuildSkyboxFromCubemap(YgString directory, Yeager::ImageExtension ext);
  bool BuildSkyboxFrom2DTexture(YgString path);
  bool BuildSkyboxFromImport(YgString path);
  void Draw(Yeager::Shader* shader, YgMatrix4 view, YgMatrix4 projection);

 private:
  void Setup();
  void SetupModel();
  void GenerateGeometry();
  void GenerateCubemapGeometry();

  bool m_SkyboxDataLoaded = false;
  bool m_ImageFlip = false;
  unsigned int m_VerticesIndex = 0;
  GLuint m_Vao = -1, m_Vbo = -1, m_Ebo = -1, m_ID = -1;

  SkyboxTextureType m_Type;
  ObjectGeometryData m_Data;
  ObjectModelData m_Model;
  ObjectGeometryType m_Geometry;
  ApplicationCore* m_Application = YEAGER_NULLPTR;
  std::shared_ptr<ToolBoxObject> m_Toolbox = YEAGER_NULLPTR;
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