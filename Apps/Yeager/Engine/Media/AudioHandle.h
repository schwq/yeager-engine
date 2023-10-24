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
class AudioHandle : public GameEntity {
 public:
  AudioHandle(yg_string path, yg_string name, bool looped);
  ~AudioHandle();

  void SetVolume(irrklang::ik_f32 volume);
  void Play();
  void Stop();
  void Pause();
  void Resume();
  void Update();

  irrklang::ik_u32 GetLenght();
  void SetSoundPos(irrklang::ik_f32 pos);
  irrklang::ik_u32 GetSoundPos();
  irrklang::ISoundEngine* GetEngine();

 private:
  irrklang::ISoundEngine* m_engine;
  irrklang::ik_u32 m_paused_play_pos;
  irrklang::ISound* m_sound;
  yg_string m_path;
  bool m_looped = false;
  bool m_paused = false;
  bool m_stopped = true;
};

}  // namespace Yeager
extern std::vector<std::shared_ptr<Yeager::AudioHandle>> m_audios;