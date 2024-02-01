//    Yeager Engine, free and open source 3D/2D renderer written in OpenGL
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

#pragma comment(lib, "irrKlang.lib")  // link with irrKlang.dll

#include "../../Common/Common.h"
#include "../../Common/LogEngine.h"
#include "../Renderer/Entity.h"

namespace Yeager {

class ApplicationCore;

struct AudioEngineHandle {
  bool InitAudioEngine();
  void TerminateAudioEngine();
  irrklang::ISoundEngine* Engine = YEAGER_NULLPTR;
  irrklang::vec3df ListernerPos = irrklang::vec3df(0.0f, 0.0f, 0.0f);
  irrklang::vec3df GetListernerPos();
  void SetListernerPos(irrklang::vec3df pos, irrklang::vec3df lookDir, irrklang::vec3df velocity,
                       irrklang::vec3df upVec);
};

enum AudioHandleSoundEffects {
  Chorus,
  Compressor,
  Distortion,
  Echo,
  Flanger,
  Gargle,
  I3DL2Reverb,
  ParamEq,
  WavesReverb
};

class AudioHandle : public GameEntity {
 public:
  AudioHandle(YgString path, YgString name, AudioEngineHandle* handle, bool looped, Yeager::ApplicationCore* app = YEAGER_NULLPTR);
  ~AudioHandle();

  void SetVolume(irrklang::ik_f32 volume);
  bool isPlaying();
  virtual void Play();
  void Stop();
  void StopAll();
  void Pause();
  virtual void Resume();
  void Update();

  irrklang::ik_u32 GetLenght();
  void SetSoundPos(irrklang::ik_f32 pos);
  irrklang::ik_u32 GetSoundPos();
  YEAGER_NODISCARD irrklang::ISoundEngine* GetEngine();
  void EnableSoundEffect(AudioHandleSoundEffects effect);
  void DisableSoundEffect(AudioHandleSoundEffects effect);
  void DisableAllSoundEffects();
  bool IsSoundEffectEnable(AudioHandleSoundEffects effect);
  bool CheckIfSoundEffectIsSupported();

  YgString GetPath() { return m_path; }

 protected:
  irrklang::ik_u32 m_paused_play_pos;
  irrklang::ISound* m_sound = YEAGER_NULLPTR;
  irrklang::ISoundSource* m_sound_source = YEAGER_NULLPTR;
  irrklang::ISoundEffectControl* m_sound_effects = YEAGER_NULLPTR;
  AudioEngineHandle* m_EngineHandle = YEAGER_NULLPTR;
  YgString m_path;
  bool m_looped = false;
  bool m_paused = false;
  bool m_stopped = true;
};

extern YgVector3 Vec3df_to_YgVec3(irrklang::vec3df vec);
extern irrklang::vec3df YgVec3_to_Vec3df(YgVector3 vec);

class Audio3DHandle : public AudioHandle {
 public:
  Audio3DHandle(YgString path, YgString name, AudioEngineHandle* handle, bool looped, irrklang::vec3df position, Yeager::ApplicationCore* app = YEAGER_NULLPTR);
  ~Audio3DHandle();

  void Play();
  void Resume();
  irrklang::vec3df GetIrrklangPosition();
  YgVector3 GetVector3Position();
  void SetAudioPos(irrklang::vec3df pos);

 private:
  irrklang::vec3df m_audio_pos = irrklang::vec3df(0.0f, 0.0f, 0.0f);
};

}  // namespace Yeager