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

#include "Common/Utils/Common.h"
#include "Common/Utils/LogEngine.h"
#include "Common/Utils/Utilities.h"

#include "Components/Renderer/Objects/Entity.h"

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
  NodeComponent(Yeager::ApplicationCore* application, Yeager::EditorEntity* entity,
                std::shared_ptr<NodeComponent> parent);

  bool AddChild(std::shared_ptr<NodeComponent> child);
  bool AddChild(const std::vector<std::shared_ptr<NodeComponent>>& child);
  bool AddChild(Yeager::EditorEntity* entity);
  bool AddChild(Yeager::NodeComponent&& child);

  bool RemoveChild(std::shared_ptr<NodeComponent> child);

  bool LinkToParent(std::shared_ptr<NodeComponent> parent);

  // copy assignment
  Yeager::NodeComponent& operator=(const Yeager::NodeComponent& other);

  std::shared_ptr<NodeComponent> GetParent()
  {
    if (!m_IsRoot)
      return m_Parent;
    return std::make_shared<NodeComponent>(*this);
  }

  /* Just for the non root nodes */
  Yeager::EditorEntity* GetEntity();

  /* Just for the root node */
  Yeager::Scene* GetRootScene();

  std::vector<std::shared_ptr<NodeComponent>>* GetChildren() { return &m_Children; }
  constexpr YEAGER_FORCE_INLINE bool IsRoot() const { return m_IsRoot; }

 private:
  Yeager::ApplicationCore* m_Application = YEAGER_NULLPTR;
  std::shared_ptr<NodeComponent> m_Parent = YEAGER_NULLPTR;
  Yeager::EditorEntity* m_Entity = YEAGER_NULLPTR;
  Yeager::Scene* m_RootScene = YEAGER_NULLPTR;
  std::vector<std::shared_ptr<NodeComponent>> m_Children;

  /* There can be only one node that is the root of the project! */
  bool m_IsRoot = false;
};

extern void AddNodeComponentToScene(std::shared_ptr<NodeComponent> node, Yeager::Scene* scene);
/* Checks if isChild is a child node of ofParent and returns a boolean */
extern bool IsNodeChildOf(std::shared_ptr<NodeComponent> isChild, std::shared_ptr<NodeComponent> ofParent);
extern bool IsNodeParent(std::shared_ptr<NodeComponent> isParent);

extern bool MoveNodeChildToNewParent(std::shared_ptr<NodeComponent> child, std::shared_ptr<NodeComponent> newParent);
extern bool CopyNodeChildToNewParent(std::shared_ptr<NodeComponent> child, std::shared_ptr<NodeComponent> newParent);

/* The delete child of node, clears EVERY node! it is a recusive function that keeps cleaning children and left only the root node */
extern void DeleteChildOf(std::shared_ptr<NodeComponent> node);
extern void DeleteChildOf(std::shared_ptr<NodeComponent> node);
extern void SwapNodes(std::shared_ptr<NodeComponent> node1, std::shared_ptr<NodeComponent> node2);

}  // namespace Yeager
