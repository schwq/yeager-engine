#include "ToolboxObj.h"
#include "../Interface/IconsFontAwesome6.h"
#include "../Interface/Interface.h"
#include "../Renderer/Entity.h"
#include "../Renderer/Object.h"
using namespace ImGui;
using namespace Yeager;

YgString Yeager::ExplorerTypeToString(ExplorerObjectType type)
{
  switch (type) {
    case EExplorerTypeObject:
      return "Object";
    case EExplorerTypeAnimatedObject:
      return "Animated Object";
    case EExplorerTypeShader:
      return "Shader";
    case EExplorerTypeSkybox:
      return "Skybox";
    case EExplorerTypeInstancedObject:
      return "Instanced Object";
    case EExplorerTypeAnimatedInstancedObject:
      return "Animated Instanced Object";
    case EExplorerTypeAudio:
      return "Audio";
    case EExplorerTypeAudio3D:
      return "Audio 3D";
    case EExplorerTypeNull:
      return "NULL";
    default:
      return "ERROR_NOT_DEFINED";
  }
}

void ToolBoxObject::DrawObject()
{
  switch (m_ExplorerType) {
    case EExplorerTypeObject:
      DrawToolboxObjectType();
      break;
    case EExplorerTypeAnimatedObject:
      DrawToolboxObjectAnimated();
      break;
    case EExplorerTypeAudio:
      DrawToolboxAudio();
      break;
    case EExplorerTypeAudio3D:
      DrawToolboxAudio3D();
      break;
    default:
      Yeager::Log(ERROR, "No type was declated for this entity!");
      assert(0);
  }
}

void ToolBoxObject::DrawToolboxObjectType()
{
  Yeager::Object* obj = (Yeager::Object*)m_GameEntityPtr;
  if (!obj->IsLoaded() || !obj->IsValid())
    return;

  EntityPhysics* physics = obj->GetPhysics();
  Transformation* trans = obj->GetTransformationPtr();

  if (Button("Delete")) {
    obj->SetScheduleDeletion(true);
  }

  Text("Position:");
  InputVector3("Position", &trans->position);
  if (Button("Reset Position")) {
    trans->position = YgVector3(0.0f);
  }

  Text("Rotation");
  SameLine();
  Checkbox("Random Rotation", &m_RandomRotationCheckbox);
  InputFloat("Random Rotation Power", &m_RandomRotationPower);
  if (m_RandomRotationCheckbox) {
    float angles = glfwGetTime();
    trans->rotation.x = 10.0f * angles * m_RandomRotationPower;
    trans->rotation.y = 20.0f * angles * m_RandomRotationPower;
    trans->rotation.z = 15.0f * angles * m_RandomRotationPower;
  }
  InputVector3("Rotation", &trans->rotation);
  InputVector3("Scale", &trans->scale);

  Checkbox("Gravity Enabled", &m_PhysicGravityCheckbox);
  physics->GravityEnabled(m_PhysicGravityCheckbox);
  Checkbox("Reverse Gravity", &m_PhysicReverseGravityCheckbox);
  if (m_PhysicReverseGravityCheckbox) {
    physics->GravityReversed(true);
  } else {
    physics->GravityReversed(false);
  }
  InputFloat("Weight", &m_ObjectPhysicWeight);
  InputFloat("Gravity Const", &m_ObjectPhysicGravityConst);
  physics->ChangeGravity(m_ObjectPhysicGravityConst, m_ObjectPhysicWeight);

  for (auto& tex : *m_GameEntityPtr->GetLoadedTextures()) {

    ImGui::Text("%s", tex->Name.c_str());
    ImGui::Text("%s", tex->Type.c_str());
    Image((void*)(intptr_t)tex->ID, ImVec2(300, 300));
  }
}

void ToolBoxObject::DrawToolboxObjectAnimated()
{
  DrawToolboxObjectType();
}

void ToolBoxObject::DrawToolboxAudio()
{
  Yeager::AudioHandle* m_audio = (Yeager::AudioHandle*)m_GameEntityPtr;

  YgString extensionName = ExtractExtensionTypenameFromPath(m_audio->GetPath());
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
  SliderFloat(ICON_FA_VOLUME_HIGH " Volume", &m_AudioVolume, 0.0f, 1.0f);
  m_audio->SetVolume(m_AudioVolume);

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

void ToolBoxObject::DrawToolboxAudio3D()
{
  DrawToolboxAudio();
  Yeager::Audio3DHandle* m_audio = (Yeager::Audio3DHandle*)m_GameEntityPtr;
  InputFloat3("Audio Position", m_Audio3DPosition);
  irrklang::vec3df position = irrklang::vec3df(m_Audio3DPosition[0], m_Audio3DPosition[1], m_Audio3DPosition[2]);
  m_audio->SetAudioPos(position);
}
