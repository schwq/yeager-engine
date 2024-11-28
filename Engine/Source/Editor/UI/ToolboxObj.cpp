#include "ToolboxObj.h"
#include "Components/Renderer/AnimationEngine/AnimationEngine.h"
#include "Components/Renderer/Objects/Object.h"
#include "Components/Renderer/Skybox/Skybox.h"
#include "Components/Renderer/Texture/TextureHandle.h"
#include "IconsFontAwesome6.h"
#include "Interface.h"
using namespace ImGui;
using namespace Yeager;

bool Yeager::ValidateToolbox(ToolboxHandle* handle)
{
  bool isNull = (handle == YEAGER_NULLPTR);
  if (!isNull) {
    return (handle->GetEntityType() != ToolboxEntityType::Undefined);
  }
  return false;
}

void ToolboxHandle::DrawObject()
{
  switch (m_EntityPtr->GetEntityType()) {
    case EntityObjectType::OBJECT:
      DrawToolboxObjectType();
      break;
    case EntityObjectType::OBJECT_ANIMATED:
      DrawToolboxObjectAnimated();
      break;
    case EntityObjectType::AUDIO_HANDLE:
      DrawToolboxAudio();
      break;
    case EntityObjectType::AUDIO_3D_HANDLE:
      DrawToolboxAudio3D();
      break;
    case EntityObjectType::SKYBOX:
      DrawToolboxSkybox();
      break;
    case EntityObjectType::OBJECT_PLAYABLE:
      DrawToolboxObjectPlayable();
      break;
    case EntityObjectType::OBJECT_ANIMATED_PLAYABLE:
      DrawToolboxObjectAnimatedPlayable();
      break;
    default:
      Yeager::Log(ERROR, "No type was declated for this entity!");
      assert(0);
  }
}

ToolboxHandle::ToolboxHandle(Yeager::EditorEntity* entity)
    : m_EntityPtr(entity), m_EntityType(ToolboxEntityType::EditorType)
{
  m_IsSeen = EntityToolboxIsSeen(entity->GetEntityType());
}

ToolboxHandle::ToolboxHandle(Yeager::GameEntity* entity)
    : m_EntityPtr(static_cast<Yeager::EditorEntity*>(entity)), m_EntityType(ToolboxEntityType::GameType)
{
  m_IsSeen = EntityToolboxIsSeen(entity->GetEntityType());
}

bool Yeager::EntityToolboxIsSeen(EntityObjectType::Enum type)
{
  switch (type) {  // We just need to declare the true ones
    case EntityObjectType::AUDIO_3D_HANDLE:
    case EntityObjectType::AUDIO_HANDLE:
    case EntityObjectType::OBJECT:
    case EntityObjectType::OBJECT_ANIMATED:
    case EntityObjectType::OBJECT_INSTANCED:
    case EntityObjectType::OBJECT_INSTANCED_ANIMATED:
    case EntityObjectType::OBJECT_ANIMATED_PLAYABLE:
    case EntityObjectType::OBJECT_PLAYABLE:
    case EntityObjectType::SKYBOX:
      return true;
    default:
      return false;
  }
}

void ToolboxHandle::DrawToolboxObjectType()
{
  if (ValidateToolbox(this)) {
    Yeager::Object* obj = (Yeager::Object*)m_EntityPtr;
    if (!obj->IsLoaded())
      return;

    Transformation3D* trans = obj->GetTransformationPtr();

    if (Button("Rename")) {
      m_RenameWindowOpen = true;
    }
    if (m_RenameWindowOpen) {

      Yeager::ApplicationCore* application = obj->GetApplication();
      const auto windowSize = ImGui::GetWindowSize();
      const auto windowPos = ImGui::GetWindowPos();

      application->GetInput()->SetCameraCursorToWindowState(true);

      SetNextWindowSize(ImVec2(windowSize.x, 50));
      SetNextWindowPos(windowPos);

      Begin("Rename Object");

      if (m_NewName == YEAGER_NULL_LITERAL)
        m_NewName = obj->GetName();
      (InputText("New name", &m_NewName));
      SameLine();
      if (Button("Apply")) {
        if (!(m_NewName.size() <= 0)) {
          obj->SetName(m_NewName);
          m_RenameWindowOpen = false;
          m_NewName = YEAGER_NULL_LITERAL;
          application->GetInput()->SetCameraCursorToWindowState(false);
        }
      }

      End();
    }

    SameLine();

    if (Button("Delete")) {
      obj->SetScheduleDeletion(true);
    }

    Text("Position:");
    InputVector3("Position", &trans->position);
    if (Button("Reset Position")) {
      trans->position = Vector3(0.0f);
    }
    SameLine();
    if (Button("Reset Scale")) {
      trans->scale = Vector3(1.0f);
    }
    SameLine();
    if (Button("Reset Rotation")) {
      trans->rotation = Vector3(0.0f);
    }

    if (obj->GetGeometry() == ObjectGeometryType::eCUSTOM) {
      if (Button("Regenerate Textures and flip")) {
        m_FlipEveryTexture = !m_FlipEveryTexture;
        if (m_EntityPtr->GetEntityType() == EntityObjectType::OBJECT) {
          for (const auto& p : obj->GetModelData()->TexturesLoaded) {
            Yeager::MaterialTexture2D* texture = &p->first;
            texture->RegenerateTextureFlipped(m_FlipEveryTexture);
          }

        } else if (m_EntityPtr->GetEntityType() == EntityObjectType::OBJECT_ANIMATED) {
          Yeager::AnimatedObject* animated = static_cast<Yeager::AnimatedObject*>(obj);
          for (const auto& p : animated->GetModelData()->TexturesLoaded) {
            Yeager::MaterialTexture2D* texture = &p->first;
            texture->RegenerateTextureFlipped(m_FlipEveryTexture);
          }
        }
      }
    }

    Text("Rotation");
    SameLine();
    Checkbox("Random Rotation", &m_Options.m_RandomRotationCheckbox);
    InputFloat("Random Rotation Power", &m_Options.m_RandomRotationPower);
    if (m_Options.m_RandomRotationCheckbox) {
      float angles = glfwGetTime();
      trans->rotation.x = 10.0f * angles * m_Options.m_RandomRotationPower;
      trans->rotation.y = 20.0f * angles * m_Options.m_RandomRotationPower;
      trans->rotation.z = 15.0f * angles * m_Options.m_RandomRotationPower;
    }
    InputVector3("Rotation", &trans->rotation);
    InputVector3("Scale", &trans->scale);

    Checkbox("Culling Enabled", &obj->GetOnScreenProprieties()->m_CullingEnabled);

    if (Button("PolygonMode: FILL")) {
      obj->GetOnScreenProprieties()->m_PolygonMode = RenderingGLPolygonMode::eFILL;
    }

    SameLine();

    if (Button("PolygonMode: LINE")) {
      obj->GetOnScreenProprieties()->m_PolygonMode = RenderingGLPolygonMode::eLINES;
    }

    SameLine();

    if (Button("PolygonMode: POINT")) {
      obj->GetOnScreenProprieties()->m_PolygonMode = RenderingGLPolygonMode::ePOINTS;
    }

    SliderInt("Resize textures", (int*)&m_ResizeTextures, 1, 100);

    if (m_EntityPtr->GetEntityType() == EntityObjectType::OBJECT) {
      for (const auto& p : obj->GetModelData()->TexturesLoaded) {
        Yeager::MaterialTexture2D* texture = &p->first;
        DisplayTextureInformation(texture);
      }
    }
  }
}

void ToolboxHandle::DisplayTextureInformation(MaterialTexture2D* texture)
{
  if (texture->IsGenerated()) {
    Text(texture->GetName().c_str());
    Text("Flipped: %s", texture->GetTextureDataHandle()->Flipped ? "True" : "False");
    Text("Width %u, Height %u", texture->GetWidth(), texture->GetHeight());
    DisplayImageImGui(texture, m_ResizeTextures);
  }
}

void ToolboxHandle::DrawToolboxObjectAnimated()
{
  DrawToolboxObjectType();
  Yeager::AnimatedObject* obj = static_cast<Yeager::AnimatedObject*>(m_EntityPtr);
  if (obj->IsLoaded()) {
    for (auto& animation : *obj->GetAnimationEngine()->GetAnimations()) {
      Text("Name: %s", animation.GetName().c_str());
      Text("Duration: %f", animation.GetDuration());
      if (Button("Play")) {
        Uint index = animation.GetIndex();
        obj->GetAnimationEngine()->PlayAnimation(index);
      }
    }
  }
}

void ToolboxHandle::DrawToolboxAudio()
{
  Yeager::AudioHandle* m_audio = (Yeager::AudioHandle*)m_EntityPtr;

  String extensionName = ExtractExtensionTypenameFromPath(m_audio->GetPath());
  Text("Extension %s : %s", ExtractExtensionFromFilename(m_audio->GetPath()).c_str(), extensionName.c_str());
  Text("Driver name %s", m_audio->GetEngine()->getDriverName());
  if (Button(ICON_FA_PLAY " Play")) {
    m_audio->Play();
  }
  SameLine();
  if (Button(ICON_FA_STOP " Stop")) {
    m_audio->Stop();
  }
  SameLine();
  if (Button(ICON_FA_PAUSE " Pause")) {
    m_audio->Pause();
  }
  SameLine();
  if (Button(ICON_FA_MUSIC " Resume")) {
    m_audio->Resume();
  }
  if (Button(ICON_FA_FORWARD " Forward 10s")) {
    m_audio->SetSoundPos(m_audio->GetSoundPos() + 10 * 1000);
  }
  SameLine();
  if (Button(ICON_FA_BACKWARD " Back 10s")) {
    m_audio->SetSoundPos(m_audio->GetSoundPos() - 10 * 1000);
  }
  SliderFloat(ICON_FA_VOLUME_HIGH " Volume", &m_Options.m_AudioVolume, 0.0f, 1.0f);
  m_audio->SetVolume(m_Options.m_AudioVolume);

  Uint time_reimander = (m_audio->GetLenght() - m_audio->GetSoundPos()) / 1000;
  Uint seconds = time_reimander % 60;
  Uint minutes = time_reimander / 60;
  if (seconds < 10) {
    Text("Time reimander: %u:0%u", minutes, seconds);
  } else {
    Text("Time reimander: %u:%u", minutes, seconds);
  }
}

void ToolboxHandle::DrawToolboxAudio3D()
{
  DrawToolboxAudio();
  Yeager::Audio3DHandle* m_audio = (Yeager::Audio3DHandle*)m_EntityPtr;
  InputFloat3("Audio Position", m_Options.m_Audio3DPosition);
  irrklang::vec3df position =
      irrklang::vec3df(m_Options.m_Audio3DPosition[0], m_Options.m_Audio3DPosition[1], m_Options.m_Audio3DPosition[2]);
  m_audio->SetAudioPos(position);
}

void ToolboxHandle::DrawToolboxSkybox()
{
  Yeager::Skybox* skybox = (Yeager::Skybox*)m_EntityPtr;
  const ObjectGeometryType::Enum type = skybox->GetGeometry();

  SliderInt("Resize textures", (int*)&m_ResizeTextures, 1, 100);
  Yeager::MaterialTexture2D* texture = &skybox->GetModelData()->TexturesLoaded[0]->first;
  DisplayTextureInformation(texture);
}

void ToolboxHandle::DrawToolboxObjectPlayable()
{
  DrawToolboxObjectType();
  Yeager::PlayableObject* playable = static_cast<PlayableObject*>(m_EntityPtr);
  Yeager::PlayableSetOfRules* rules = playable->GetSetOfRules();

  Checkbox("Follow Player Position", &rules->FollowPlayerPosition);
  Checkbox("Follow Player Rotation", &rules->FollowPlayerRotation);
  Checkbox("Follow Player Scale", &rules->FollowPlayerScale);
  Checkbox("Look At Position", &rules->LookAtPosition);

  Separator();

  InputVector3("Look At", &rules->PositionToLookAt);
  InputVector3("Look At Offset", &rules->PositonToLookAtOffset);
  InputVector3("Follow Player Position", &rules->PositionOffset);
  InputVector3("Follow Player Rotation", &rules->RotationOffset);
  InputVector3("Follow Player Scale", &rules->ScaleOffset);
}

void ToolboxHandle::DrawToolboxObjectAnimatedPlayable()
{
  DrawToolboxObjectAnimated();

  Yeager::PlayableAnimatedObject* playable = static_cast<PlayableAnimatedObject*>(m_EntityPtr);
  Yeager::PlayableSetOfRules* rules = playable->GetSetOfRules();

  Checkbox("Follow Player Position", &rules->FollowPlayerPosition);
  Checkbox("Follow Player Rotation", &rules->FollowPlayerRotation);
  Checkbox("Follow Player Scale", &rules->FollowPlayerScale);
  Checkbox("Look At Position", &rules->LookAtPosition);

  Separator();

  InputVector3("Look At", &rules->PositionToLookAt);
  InputVector3("Look At Offset", &rules->PositonToLookAtOffset);
  InputVector3("Follow Player Position", &rules->PositionOffset);
  InputVector3("Follow Player Rotation", &rules->RotationOffset);
  InputVector3("Follow Player Scale", &rules->ScaleOffset);
}