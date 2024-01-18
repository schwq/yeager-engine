#include "ToolboxObj.h"
#include "../Interface/IconsFontAwesome6.h"
#include "../Interface/Interface.h"
#include "../Renderer/Entity.h"
#include "../Renderer/Object.h"
using namespace ImGui;
using namespace Yeager;

void ToolBoxObject::DrawObject()
{
  switch (m_type) {
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
  Yeager::Object* obj = (Yeager::Object*)m_entity;
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
  Checkbox("Random Rotation", &m_random_rotation_checkbox);
  InputFloat("Random Rotation Power", &m_random_rotation_pow);
  if (m_random_rotation_checkbox) {
    float angles = glfwGetTime();
    trans->rotation.x = 10.0f * angles * m_random_rotation_pow;
    trans->rotation.y = 20.0f * angles * m_random_rotation_pow;
    trans->rotation.z = 15.0f * angles * m_random_rotation_pow;
  }
  InputVector3("Rotation", &trans->rotation);
  InputVector3("Scale", &trans->scale);

  Checkbox("Gravity Enabled", &m_gravity_checkbox);
  physics->GravityEnabled(m_gravity_checkbox);
  Checkbox("Reverse Gravity", &m_reverse_gravity_checkbox);
  if (m_reverse_gravity_checkbox) {
    physics->GravityReversed(true);
  } else {
    physics->GravityReversed(false);
  }
  InputFloat("Weight", &obj_weight);
  InputFloat("Gravity Const", &obj_gravity_const);
  physics->ChangeGravity(obj_gravity_const, obj_weight);

  for (auto& tex : *m_entity->GetLoadedTextures()) {

    ImGui::Text("%s", tex->Name.c_str());
    ImGui::Text("%s", tex->Type.c_str());
    Image((void*)(intptr_t)tex->ID, ImVec2(300, 300));
  }
}

void ToolBoxObject::DrawToolboxObjectAnimated() {}

void ToolBoxObject::DrawToolboxAudio()
{
  Yeager::AudioHandle* m_audio = (Yeager::AudioHandle*)m_entity;

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
  SliderFloat(ICON_FA_VOLUME_HIGH " Volume", &m_sound_volume, 0.0f, 1.0f);
  m_audio->SetVolume(m_sound_volume);

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
  Yeager::Audio3DHandle* m_audio = (Yeager::Audio3DHandle*)m_entity;
  InputFloat3("Audio Position", m_3d_audio_position);
  irrklang::vec3df position = irrklang::vec3df(m_3d_audio_position[0], m_3d_audio_position[1], m_3d_audio_position[2]);
  m_audio->SetAudioPos(position);
}
