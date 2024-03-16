#include "ToolboxObj.h"
#include "../Interface/IconsFontAwesome6.h"
#include "../Interface/Interface.h"
#include "../Renderer/Entity.h"
#include "../Renderer/Object.h"
#include "../Renderer/Skybox.h"
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

    Transformation* trans = obj->GetTransformationPtr();

    if (Button("Delete")) {
      obj->SetScheduleDeletion(true);
    }

    Text("Position:");
    InputVector3("Position", &trans->position);
    if (Button("Reset Position")) {
      trans->position = Vector3(0.0f);
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
  }
}

void ToolboxHandle::DrawToolboxObjectAnimated()
{
  DrawToolboxObjectType();
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

  unsigned int time_reimander = (m_audio->GetLenght() - m_audio->GetSoundPos()) / 1000;
  unsigned int seconds = time_reimander % 60;
  unsigned int minutes = time_reimander / 60;
  if (seconds < 10) {
    Text("Time reimander: %u:0%u", minutes, seconds);
  } else {
    Text("Time reimander: %u:%u", minutes, seconds);
  }

  if (Button("Add Waves")) {
    m_audio->EnableSoundEffect(Yeager::WavesReverb);
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
}
