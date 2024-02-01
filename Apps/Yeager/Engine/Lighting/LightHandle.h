//    Yeager Engine, free and open source 3D/2D renderer written in OpenGL
//    In case of questions and bugs, please, refer to the issue tab on github
//    Repo : https://github.com/schwq/yeager-engine
//    Copyright (C) 2023-present
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
#include "../../Common/Mathematics.h"
#include "../../Common/Utilities.h"
#include "../Renderer/Object.h"
#include "../Renderer/ShaderHandle.h"
#include "../Renderer/TextureHandle.h"

#define MAX_POINT_LIGHTS 10

namespace Yeager {

class ApplicationCore;

struct PointLight {
  YgVector3 Position = YgVector3(0.0f);
  float Constant = 1.0f;
  float Linear = 0.09f;
  float Quadratic = 0.032f;
  YgVector3 Ambient = YgVector3(0.05f);
  YgVector3 Diffuse = YgVector3(0.8f);
  YgVector3 Specular = YgVector3(1.0f);
  YgVector3 Color = YgVector3(1.0f);
  bool Active = false;
};

struct DirectionalLight {
  YgVector3 Direction = YgVector3(-0.2f, -1.0f, -0.3f);
  YgVector3 Ambient = YgVector3(0.03f);
  YgVector3 Diffuse = YgVector3(0.4f);
  YgVector3 Specular = YgVector3(0.5f);
  YgVector3 Color = YgVector3(1.0f);
};

struct Material {
  float Shininess = 32.0f;
};

struct Viewer {
  YgVector3 Position = YgVector3(0.0f);
};

struct SpotLight {
  YgVector3 Position = YgVector3(0.0f);
  YgVector3 Direction = YgVector3(0.0f);
  float CutOff = glm::cos(glm::radians(12.5f));
  float OuterCutOff = glm::cos(glm::radians(15.0f));
  float Constant = 1;
  float Linear = 0.09f;
  float Quadratic = 0.032f;
  YgVector3 Ambient = YgVector3(0.0f);
  YgVector3 Diffuse = YgVector3(1.0f);
  YgVector3 Specular = YgVector3(1.0f);
  bool Active = true;
};

struct ObjectPointLight : public PointLight {
  Object* ObjSource = YEAGER_NULLPTR;
  constexpr void Destroy() const noexcept
  {
    if (ObjSource)
      delete ObjSource;
  }
};

class LightBaseHandle : public EditorEntity {
 public:
  LightBaseHandle(YgString name, ApplicationCore* app, std::vector<Shader*> link_shaders);
  ~LightBaseHandle() {}

  std::vector<PointLight>* GetPointLights() { return &m_PointLights; }
  DirectionalLight* GetDirectionalLight() { return &m_DirectionalLight; }
  Material* GetMaterial() { return &m_Material; }
  Viewer* GetViewer() { return &m_Viewer; }
  SpotLight* GetSpotLight() { return &spotLight; }
  virtual void BuildShaderProps(YgVector3 viewPos, YgVector3 front, float shininess);

  /* Returns linked shaders, that are the shaders affected by the class lighting management */
  std::vector<Shader*>* GetLinkedShaders() { return &m_LinkedShader; }

 protected:
  std::vector<PointLight> m_PointLights;
  std::vector<Shader*> m_LinkedShader;
  DirectionalLight m_DirectionalLight;
  Material m_Material;
  Viewer m_Viewer;
  SpotLight spotLight;
};

/**
* @brief PhysicalLightHandle is inheritance from LightBaseHandle, that actually does mostly things the same, but the light sources in the scene
* are drawn like models or geometries
*/
class PhysicalLightHandle : public LightBaseHandle {
 public:
  PhysicalLightHandle(YgString name, ApplicationCore* app, std::vector<Shader*> link_shader, Shader* draw_shader);
  ~PhysicalLightHandle();

  bool operator==(const PhysicalLightHandle& rhs) { return (this->m_Name == rhs.m_Name); }

  void ScheduleDeletionOfPointLights()
  {
    for (const auto& obj : m_ObjectPointLights) {
      obj.ObjSource->GetToolbox()->SetScheduleDeletion(true);
      obj.ObjSource->SetScheduleDeletion(true);
    }
  }

  void AddObjectPointLight(const ObjectPointLight& obj, Transformation& trans);
  void AddObjectPointLight(const ObjectPointLight& obj, Yeager::Object& custom_obj);
  void AddObjectPointLight(const ObjectPointLight& obj);
  void AddObjectPointLight(ObjectPointLight* light, ObjectGeometryType type);
  void BuildShaderProps(YgVector3 viewPos, YgVector3 front, float shininess);
  void DrawLightSources();

  /* Returns the pointer to shader which is used to draw the light sources in the scene */
  Shader* GetDrawableShader() const { return m_DrawableShader; }

  /* Returns the vector of objects that acts like light sources in the scene */
  std::vector<ObjectPointLight>* GetObjectPointLights() { return &m_ObjectPointLights; }

 protected:
  std::vector<ObjectPointLight> m_ObjectPointLights;
  Shader* m_DrawableShader = YEAGER_NULLPTR;
};
}  // namespace Yeager