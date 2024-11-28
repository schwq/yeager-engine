#include "NodeHierarchy.h"
#include "Main/Core/Application.h"
#include "Main/Scene/Scene.h"
using namespace Yeager;

void Yeager::SwapNodes(std::shared_ptr<NodeComponent> node1, std::shared_ptr<NodeComponent> node2)
{
  Yeager::NodeComponent tmp = *node1;
  *node1 = *node2;
  *node2 = tmp;
}

void Yeager::DeleteChildOf(std::shared_ptr<NodeComponent> node)
{
  if (!node)
    return;

  for (auto& child : *node->GetChildren()) {
    if (child == YEAGER_NULLPTR)
      continue;
    if (IsNodeParent(child)) {
      DeleteChildOf(child);
    }
    child.reset();
  }
  node->GetChildren()->clear();
}

void Yeager::AddNodeComponentToScene(std::shared_ptr<NodeComponent> node, Yeager::Scene* scene)
{
  scene->GetNodeHierarchy()->push_back(node);
}

bool Yeager::IsNodeChildOf(std::shared_ptr<NodeComponent> isChild, std::shared_ptr<NodeComponent> ofParent)
{
  if (!IsNodeParent(ofParent))
    return false;
  for (const auto& child : *ofParent->GetChildren()) {
    if (child == isChild)
      return true;
  }
  return false;
}

bool Yeager::MoveNodeChildToNewParent(std::shared_ptr<NodeComponent> child, std::shared_ptr<NodeComponent> newParent)
{
  if (IsNodeChildOf(child, newParent))
    return true;

  std::shared_ptr<NodeComponent> oldParent = child->GetParent();

  if (!newParent->AddChild(child))
    return false;

  if (!oldParent->RemoveChild(child))
    return false;

  return true;
}

bool NodeComponent::RemoveChild(std::shared_ptr<NodeComponent> child)
{
  for (Uint x = 0; x < m_Children.size(); x++) {
    std::shared_ptr<NodeComponent> c = m_Children.at(x);
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

bool Yeager::CopyNodeChildToNewParent(std::shared_ptr<NodeComponent> child, std::shared_ptr<NodeComponent> newParent)
{
  if (IsNodeChildOf(child, newParent))
    return true;

  if (!newParent->AddChild(*&child))
    return false;

  return true;
}

bool Yeager::IsNodeParent(std::shared_ptr<NodeComponent> isParent)
{
  return (isParent->GetChildren()->size() > 0);
}

NodeComponent::NodeComponent(Yeager::ApplicationCore* application, Yeager::Scene* scene)
    : m_Application(application), m_RootScene(scene)
{
  std::shared_ptr<NodeComponent> instance = std::make_shared<NodeComponent>(*this);
  if (!m_Application->GetScene()->SceneContainsRootNode()) {
    m_Application->GetScene()->SetSceneContainsRootNode(true, instance);
    m_Parent = instance;
    m_IsRoot = true;
  } else {
    m_Parent = m_Application->GetScene()->GetRootNode();
    m_Application->GetScene()->GetRootNode()->AddChild(instance);
  }
  AddNodeComponentToScene(instance, m_Application->GetScene());
}

EditorEntity* NodeComponent::GetEntity()
{
  if (!m_IsRoot)
    return m_Entity;
  return 0;
}

NodeComponent::NodeComponent(Yeager::ApplicationCore* application, Yeager::EditorEntity* entity,
                             std::shared_ptr<NodeComponent> parent)
    : m_Application(application), m_Entity(entity), m_Parent(parent)
{
  std::shared_ptr<NodeComponent> instance = std::make_shared<NodeComponent>(*this);
  m_Parent->GetChildren()->push_back(instance);
  m_RootScene = m_Application->GetScene();
  AddNodeComponentToScene(instance, m_Application->GetScene());
}

Scene* NodeComponent::GetRootScene()
{
  if (m_IsRoot) {
    return m_RootScene;
  }
  return m_Application->GetScene()->GetRootNode()->GetRootScene();
}

bool NodeComponent::AddChild(std::shared_ptr<NodeComponent> child)
{
  std::shared_ptr<NodeComponent> instance = std::make_shared<NodeComponent>(*this);
  if (child != YEAGER_NULLPTR) {
    m_Children.push_back(child);
    child->LinkToParent(instance);
    return true;
  }
  return false;
}

bool NodeComponent::AddChild(const std::vector<std::shared_ptr<NodeComponent>>& child)
{
  std::shared_ptr<NodeComponent> instance = std::make_shared<NodeComponent>(*this);
  for (const auto& c : child) {
    c->LinkToParent(instance);
  }

  m_Children.reserve(m_Children.size() + child.size());
  m_Children.insert(m_Children.end(), child.begin(), child.end());

  return true;
}

bool NodeComponent::AddChild(Yeager::NodeComponent&& child)
{
  std::shared_ptr<NodeComponent> c = std::make_shared<NodeComponent>(std::move(child));
  m_Children.push_back(c);
  return true;
}

bool NodeComponent::AddChild(Yeager::EditorEntity* entity)
{
  std::shared_ptr<NodeComponent> instance = std::make_shared<NodeComponent>(*this);
  if (entity->GetNodeComponent() != YEAGER_NULLPTR) {
    m_Children.push_back(entity->GetNodeComponent());
    entity->GetNodeComponent()->LinkToParent(instance);
    return true;
  }
  return false;
}

bool NodeComponent::LinkToParent(std::shared_ptr<NodeComponent> parent)
{
  m_Parent = parent;
  return true;
}
