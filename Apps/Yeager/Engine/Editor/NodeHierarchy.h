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

#include "../../Common/Common.h"
#include "../../Common/LogEngine.h"
#include "../../Common/Utilities.h"
#include "../Renderer/Entity.h"

namespace Yeager {
class ApplicationCore;
class Scene;

class NodeComponent {
 public:
  /* If this constructors fails in creating a root node (mostly because there are already a root node), they automaticaly 
  become child of the root node */

  /* Root Node Constructor */
  NodeComponent(Yeager::ApplicationCore* application, Yeager::Scene* scene);

  /* Child node constructor */
  NodeComponent(Yeager::ApplicationCore* application, Yeager::EditorEntity* entity, Yeager::NodeComponent* parent);

  bool AddChild(Yeager::NodeComponent* child);
  bool AddChild(const std::vector<Yeager::NodeComponent*>& child);
  bool AddChild(Yeager::EditorEntity* entity);
  bool AddChild(Yeager::NodeComponent&& child);

  bool RemoveChild(Yeager::NodeComponent* child);

  bool LinkToParent(Yeager::NodeComponent* parent);

  Yeager::NodeComponent* GetParent()
  {
    if (!m_IsRoot)
      return m_Parent;
    return this;
  }

  /* Just for the non root nodes */
  Yeager::EditorEntity* GetEntity();

  /* Just for the root node */
  Yeager::Scene* GetRootScene();

  std::vector<Yeager::NodeComponent*>* GetChildren() { return &m_Children; }
  constexpr YEAGER_FORCE_INLINE bool IsRoot() const { return m_IsRoot; }

 private:
  Yeager::ApplicationCore* m_Application = YEAGER_NULLPTR;
  Yeager::NodeComponent* m_Parent = YEAGER_NULLPTR;
  Yeager::EditorEntity* m_Entity = YEAGER_NULLPTR;
  Yeager::Scene* m_RootScene = YEAGER_NULLPTR;
  std::vector<Yeager::NodeComponent*> m_Children;

  /* There can be only one node that is the root of the project! */
  bool m_IsRoot = false;
};

extern void AddNodeComponentToScene(Yeager::NodeComponent* node, Yeager::Scene* scene);
/* Checks if isChild is a child node of ofParent and returns a boolean */
extern bool IsNodeChildOf(Yeager::NodeComponent* isChild, Yeager::NodeComponent* ofParent);
extern bool IsNodeParent(Yeager::NodeComponent* isParent);

extern bool MoveNodeChildToNewParent(Yeager::NodeComponent* child, Yeager::NodeComponent* newParent);
extern bool CopyNodeChildToNewParent(Yeager::NodeComponent* child, Yeager::NodeComponent* newParent);

/* The delete child of node, clears EVERY node! it is a recusive function that keeps cleaning children and left only the root node */
extern void DeleteChildOf(Yeager::NodeComponent* node);

}  // namespace Yeager
