#include "NodeHierarchy.h"
#include "../../Application.h"
#include "../../Scene.h"
using namespace Yeager;

void Yeager::SwapNodes(Yeager::NodeComponent* node1, Yeager::NodeComponent* node2)
{
  Yeager::NodeComponent tmp = *node1;
  *node1 = *node2;
  *node2 = tmp;
}

void Yeager::DeleteChildOf(Yeager::NodeComponent* node)
{
  if (!node)
    return;

  for (auto& child : *node->GetChildren()) {
    if (child == YEAGER_NULLPTR)
      continue;
    if (IsNodeParent(child)) {
      DeleteChildOf(child);
    }
    YEAGER_DELETE(child);
  }
  node->GetChildren()->clear();
}

void Yeager::AddNodeComponentToScene(Yeager::NodeComponent* node, Yeager::Scene* scene)
{
  scene->GetNodeHierarchy()->push_back(node);
}

bool Yeager::IsNodeChildOf(Yeager::NodeComponent* isChild, Yeager::NodeComponent* ofParent)
{
  if (!IsNodeParent(ofParent))
    return false;
  for (const auto& child : *ofParent->GetChildren()) {
    if (child == isChild)
      return true;
  }
  return false;
}

bool Yeager::MoveNodeChildToNewParent(Yeager::NodeComponent* child, Yeager::NodeComponent* newParent)
{
  if (IsNodeChildOf(child, newParent))
    return true;

  Yeager::NodeComponent* oldParent = child->GetParent();

  if (!newParent->AddChild(child))
    return false;

  if (!oldParent->RemoveChild(child))
    return false;

  return true;
}

bool NodeComponent::RemoveChild(Yeager::NodeComponent* child)
{
  for (Uint x = 0; x < m_Children.size(); x++) {
    Yeager::NodeComponent* c = m_Children.at(x);
    if (child == c) {
      m_Children.erase(m_Children.begin() + x);
      return true;
    }
  }
  return false;
}
// TODO: review this copy
Yeager::NodeComponent& NodeComponent::operator=(const Yeager::NodeComponent& other)
{
  if (other.IsRoot())
    this->m_IsRoot = true;
  this->m_Entity = other.m_Entity;
  this->m_Parent = other.m_Parent;
  this->m_RootScene = other.m_RootScene;
  this->m_Children.clear();
  this->m_Children.reserve(other.m_Children.size());
  this->m_Children = other.m_Children;
  return *this;
}

bool Yeager::CopyNodeChildToNewParent(Yeager::NodeComponent* child, Yeager::NodeComponent* newParent)
{
  if (IsNodeChildOf(child, newParent))
    return true;

  if (!newParent->AddChild(*&child))
    return false;

  return true;
}

bool Yeager::IsNodeParent(Yeager::NodeComponent* isParent)
{
  return (isParent->GetChildren()->size() > 0);
}

NodeComponent::NodeComponent(Yeager::ApplicationCore* application, Yeager::Scene* scene)
    : m_Application(application), m_RootScene(scene)
{
  if (!m_Application->GetScene()->SceneContainsRootNode()) {
    m_Application->GetScene()->SetSceneContainsRootNode(true, this);
    m_Parent = this;
    m_IsRoot = true;
  } else {
    m_Parent = m_Application->GetScene()->GetRootNode();
    m_Application->GetScene()->GetRootNode()->AddChild(this);
  }
  AddNodeComponentToScene(this, m_Application->GetScene());
}

EditorEntity* NodeComponent::GetEntity()
{
  if (!m_IsRoot)
    return m_Entity;
  return 0;
}

NodeComponent::NodeComponent(Yeager::ApplicationCore* application, Yeager::EditorEntity* entity,
                             Yeager::NodeComponent* parent)
    : m_Application(application), m_Entity(entity), m_Parent(parent)
{
  m_Parent->GetChildren()->push_back(this);
  m_RootScene = m_Application->GetScene();
  AddNodeComponentToScene(this, m_Application->GetScene());
}

Scene* NodeComponent::GetRootScene()
{
  if (m_IsRoot) {
    return m_RootScene;
  }
  return m_Application->GetScene()->GetRootNode()->GetRootScene();
}

bool NodeComponent::AddChild(Yeager::NodeComponent* child)
{
  if (child != YEAGER_NULLPTR) {
    m_Children.push_back(child);
    child->LinkToParent(this);
    return true;
  }
  return false;
}

bool NodeComponent::AddChild(const std::vector<Yeager::NodeComponent*>& child)
{
  for (const auto& c : child) {
    c->LinkToParent(this);
  }

  m_Children.reserve(m_Children.size() + child.size());
  m_Children.insert(m_Children.end(), child.begin(), child.end());

  return true;
}

bool NodeComponent::AddChild(Yeager::NodeComponent&& child)
{
  Yeager::NodeComponent* c = new NodeComponent(std::move(child));
  m_Children.push_back(c);
  return true;
}

bool NodeComponent::AddChild(Yeager::EditorEntity* entity)
{
  if (entity->GetNodeComponent() != YEAGER_NULLPTR) {
    m_Children.push_back(entity->GetNodeComponent());
    entity->GetNodeComponent()->LinkToParent(this);
    return true;
  }
  return false;
}

bool NodeComponent::LinkToParent(Yeager::NodeComponent* parent)
{
  m_Parent = parent;
  return true;
}
