// Yeager Engine, free and open source 3D / 2D renderer written in OpenGL
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

#include <yaml-cpp/yaml.h>
#include "Common/Common.h"
#include "Common/LogEngine.h"
#include "Common/Utilities.h"
#include "Engine/Interface/Interface.h"
#include "Engine/Renderer/Entity.h"

namespace YAML {
template <>
struct convert<YgVector3> {
  static Node encode(const YgVector3& rhs)
  {
    Node node;
    node.push_back(rhs.x);
    node.push_back(rhs.y);
    node.push_back(rhs.z);
    return node;
  }

  static bool decode(const Node& node, YgVector3& rhs)
  {
    if (!node.IsSequence() || node.size() != 3) {
      return false;
    }
    rhs.x = node[0].as<float>();
    rhs.y = node[1].as<float>();
    rhs.z = node[2].as<float>();
    return true;
  }
};

template <>
struct convert<ImVec4> {
  static Node encode(const ImVec4& rhs)
  {
    Node node;
    node.push_back(rhs.x);
    node.push_back(rhs.y);
    node.push_back(rhs.z);
    node.push_back(rhs.w);
    return node;
  }

  static bool decode(const Node& node, ImVec4& rhs)
  {
    if (!node.IsSequence() || node.size() != 4) {
      return false;
    }
    rhs.x = node[0].as<float>();
    rhs.y = node[1].as<float>();
    rhs.z = node[2].as<float>();
    rhs.w = node[3].as<float>();
    return true;
  }
};

}  // namespace YAML
extern YAML::Emitter& operator<<(YAML::Emitter& out, const YgVector3& vector);

namespace Yeager {
class Scene;
class ApplicationCore;
extern std::vector<OpenProjectsDisplay> ReadProjectsToDisplay(YgString dir);

class Serialization {
 public:
  Serialization(Yeager::ApplicationCore* app);
  //Serialization() { Yeager::Log(INFO, "Default Serialization constructor was been called"); }

  ColorschemeConfig ReadColorschemeConfig();

  void ReadSceneShadersConfig(YgString path);
  void SerializeScene(Yeager::Scene* scene, YgString path);
  void DeserializeScene(Yeager::Scene* scene, YgString path);
  void ReadConf(YgString path){YEAGER_NOT_IMPLEMENTED("ReadConf")};
  void SaveConf(){YEAGER_NOT_IMPLEMENTED("SaveConf")};
  void ReadEditorVariables(YgCchar path);
  void SaveEditorVariables(YgCchar path);

  void WriteLoadedProjectsHandles(YgString externalFolder);
  void ReadLoadedProjectsHandles(YgString externalFolder);

 private:
  template <typename Type>
  void SerializeSTDVector(YAML::Emitter& out, std::vector<Type> vec, const char* key);
  Yeager::ApplicationCore* m_Application = YEAGER_NULLPTR;

  template <typename Type>
  const void inline CheckAndDeserialize(YAML::Node& node, Type& obj, YgCchar key) noexcept;
  void inline DeserializeEntity(Yeager::Scene* scene, YAML::Node& node, YAML::detail::iterator_value& entity);
  void inline DeserializeSceneInfo(Yeager::Scene* scene, YAML::Node& node);
  ObjectGeometryType inline DeserializeBasicObject(Yeager::Object* BaseClassObj, YAML::detail::iterator_value& entity);
  std::vector<Transformation> inline DeserializeObjectProperties(YAML::detail::iterator_value& entity);
  void inline SerializeBasicEntity(YAML::Emitter& out, YgString name, unsigned int id, YgString type);
  void inline SerializeObjectTransformation(YAML::Emitter& out, YgString name, Yeager::Transformation& transf) noexcept;

  template <typename Type>
  void inline SerializeObject(YAML::Emitter& out, const char* key, Type obj);
  void inline SerializeBasicObjectType(YAML::Emitter& out, Yeager::Object* obj);
  void inline SerializeBegin(YAML::Emitter& out, const char* key, YAML::EMITTER_MANIP manip);
  void inline SerializeSystemInfo(YAML::Emitter& out, Yeager::Scene* scene);
};
}  // namespace Yeager