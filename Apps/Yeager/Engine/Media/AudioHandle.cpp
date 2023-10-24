#include "AudioHandle.h"
using namespace irrklang;
using namespace Yeager;

std::vector<std::shared_ptr<Yeager::AudioHandle>> m_audios;

AudioHandle::AudioHandle(yg_string path, yg_string name, bool looped)
    : GameEntity(name), m_path(path), m_looped(looped)
{
  m_engine = createIrrKlangDevice();
  if (!m_engine) {
    Yeager::Log(ERROR, kSystem, "Cannot create AudioHandle name {} ID {}",
                m_name, m_id);
    return;
  }
}

void AudioHandle::Update()
{
  if (m_engine) {
    m_engine->update();
  }
}

AudioHandle::~AudioHandle()
{
  Yeager::Log(INFO, kSystem, "Destroying AudioHandle name {} ID {}", m_name,
              m_id);
  if (m_engine) {
    m_engine->drop();
  }
  if (m_sound) {
    m_sound->drop();
  }
}

void AudioHandle::SetVolume(ik_f32 volume)
{
  if (m_sound) {
    m_sound->setVolume(volume);
  }
}

void AudioHandle::Play()
{
  if (m_stopped) {
    m_sound = m_engine->play2D(m_path.c_str(), m_looped, false, true,
                               ESM_AUTO_DETECT, false);
    if (!m_sound) {
      Yeager::Log(
          ERROR, kSystem,
          "Cannot play music! ISound* havent been initialized! Name {} ID {}",
          m_name, m_id);
    } else {
      m_stopped = false;
    }
  }
}

void AudioHandle::Stop()
{
  if (m_engine && !m_stopped) {
    m_engine->stopAllSounds();
    m_stopped = true;
  }
}

void AudioHandle::Pause()
{
  if (!m_paused && m_engine) {
    m_paused_play_pos = m_sound->getPlayPosition();
    m_sound->stop();
    m_paused = true;
  }
}

void AudioHandle::Resume()
{
  if (m_paused && m_engine) {
    m_sound = m_engine->play2D(m_path.c_str(), m_looped, false, true,
                               ESM_AUTO_DETECT, false);
    m_sound->setPlayPosition(m_paused_play_pos);
    m_paused = false;
  }
}

irrklang::ik_u32 AudioHandle::GetLenght()
{
  if (m_sound) {
    return m_sound->getPlayLength();
  }
}

irrklang::ISoundEngine* AudioHandle::GetEngine()
{
  return m_engine;
}

irrklang::ik_u32 AudioHandle::GetSoundPos()
{
  if (m_sound && !m_paused) {
    return m_sound->getPlayPosition();
  } else if (m_sound && m_paused) {
    return m_paused_play_pos;
  }
}

void AudioHandle::SetSoundPos(irrklang::ik_f32 pos)
{
  if (m_sound && !m_paused) {
    irrklang::ik_f32 tmp = GetSoundPos();
    m_sound->setPlayPosition(pos);
    irrklang::ik_f32 time_shift = tmp - pos;
    Yeager::Log(INFO, kSystem,
                "AudioHandle name {} ID {}, had received a {}s time shift",
                m_name, m_id, -(time_shift / 1000.0f));
  }
}