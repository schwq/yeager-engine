#include "AudioHandle.h"
using namespace irrklang;
using namespace Yeager;

bool AudioEngineHandle::InitAudioEngine()
{
  if (!Engine) {
    if ((Engine = createIrrKlangDevice()) == 0) {
      Engine->setSoundVolume(1.0f);
      Yeager::Log(ERROR, "Cannot create the main Audio Engine!");
      return false;
    } else {
      Yeager::Log(INFO, "Success in creating the main Audio Engine!");
      return true;
    }
  } else {
    Yeager::Log(WARNING, "Audio Engine already created!");
    return true;
  }
}

AudioEngine::AudioEngine(Yeager::ApplicationCore* application, AudioEngineHandle* engine)
    : m_Application(application), m_SoundEngine(engine)
{}

bool EngineSoundHandle::Initialize(AudioEngineHandle* engine)
{

  if (!Generated) {
    SoundSource = engine->Engine->addSoundSourceFromFile(Path.c_str(), irrklang::ESM_AUTO_DETECT, true);
    if (!SoundSource) {
      Yeager::LogDebug(ERROR, "Cannot add sound source to the engine sound handle! File: {}", Path);
      Generated = false;
      return false;
    }
    Yeager::LogDebug(INFO, "Added the source source to the engine! File {}", Path);
    Generated = true;
    return true;
  }
  return false;
}

void EngineSoundHandle::Terminate()
{
  if (Generated)
    SoundSource->drop();

  Generated = false;
}

AudioEngine::~AudioEngine() {}

bool AudioEngine::AddSound(EngineSoundHandle& handle)
{
  for (const auto& sound : m_Sounds) {
    if (sound.Name == handle.Name) {
      Yeager::LogDebug(WARNING, "Trying to add a sound name that already exists! Existing file is Name {}, Path {}",
                       sound.Name, sound.Path);
      return false;
    }
  }

  if (handle.Initialize(m_SoundEngine)) {
    m_Sounds.push_back(handle);
    return true;
  }
  return false;
}

bool AudioEngine::PlaySound(const String& name)
{
  for (auto& sound : m_Sounds) {
    if (sound.Name == name) {
      if (!sound.Generated)
        return false;

      sound.Sound = m_SoundEngine->Engine->play2D(sound.SoundSource);
      return true;
    }
  }
  return false;
}

void AudioEngineHandle::TerminateAudioEngine()
{
  if (Engine) {
    Engine->drop();
  }
}

irrklang::vec3df AudioEngineHandle::GetListernerPos()
{
  return ListernerPos;
}

void AudioEngineHandle::SetListernerPos(irrklang::vec3df pos, irrklang::vec3df lookDir, irrklang::vec3df velocity,
                                        irrklang::vec3df upVec)
{
  ListernerPos = pos;
  Engine->setListenerPosition(pos, lookDir, velocity, upVec);
}

void AudioHandle::EnableSoundEffect(AudioHandleSoundEffects effect)
{
  if (m_sound && m_sound_effects) {
    switch (effect) {
      case Chorus:
        if (!m_sound_effects->enableChorusSoundEffect()) {
          Yeager::Log(ERROR, "Cannot Enabled Chorus effect on AudioHandle name {}, UUID {}", mName,
                      uuids::to_string(mEntityUUID));
        }
        break;
      case Compressor:
        if (!m_sound_effects->enableCompressorSoundEffect()) {
          Yeager::Log(ERROR, "Cannot Enabled Compressor effect on AudioHandle name {}, UUID {}", mName,
                      uuids::to_string(mEntityUUID));
        }
        break;
      case Distortion:
        if (!m_sound_effects->enableDistortionSoundEffect()) {
          Yeager::Log(ERROR, "Cannot Enabled Distorsion effect on AudioHandle name {}, UUID {}", mName,
                      uuids::to_string(mEntityUUID));
        }
        break;
      case Echo:
        if (!m_sound_effects->enableEchoSoundEffect()) {
          Yeager::Log(ERROR, "Cannot Enabled Echo effect on AudioHandle name {}, UUID {}", mName,
                      uuids::to_string(mEntityUUID));
        }
        break;
      case Flanger:
        if (!m_sound_effects->enableFlangerSoundEffect()) {
          Yeager::Log(ERROR, "Cannot Enabled Flanger effect on AudioHandle name {}, UUID {}", mName,
                      uuids::to_string(mEntityUUID));
        }
        break;
      case Gargle:
        if (!m_sound_effects->enableGargleSoundEffect()) {
          Yeager::Log(ERROR, "Cannot Enabled Gargle effect on AudioHandle name {}, UUID {}", mName,
                      uuids::to_string(mEntityUUID));
        }
        break;
      case I3DL2Reverb:
        if (!m_sound_effects->enableI3DL2ReverbSoundEffect()) {
          Yeager::Log(ERROR, "Cannot Enabled I3DL2Reverb effect on AudioHandle name {}, UUID {}", mName,
                      uuids::to_string(mEntityUUID));
        }
        break;
      case ParamEq:
        if (!m_sound_effects->enableParamEqSoundEffect()) {
          Yeager::Log(ERROR, "Cannot Enabled ParamEq effect on AudioHandle name {}, UUID {}", mName,
                      uuids::to_string(mEntityUUID));
        }
        break;
      case WavesReverb:
        if (!m_sound_effects->enableWavesReverbSoundEffect()) {
          Yeager::Log(ERROR, "Cannot Enabled WavesReverb effect on AudioHandle name {}, UUID {}", mName,
                      uuids::to_string(mEntityUUID));
        }
        break;
      default:
        Yeager::Log(ERROR, "Something goes so wrong here! :( invalid sound effect enum!");
    }
  } else {
    Yeager::Log(WARNING,
                "Cannot enable sound effect for AudioHandle name {} UUID {}, no m_sound or m_sound_effect initialized "
                "correctly!",
                mName, uuids::to_string(mEntityUUID));
  }
}
void AudioHandle::DisableSoundEffect(AudioHandleSoundEffects effect)
{
  if (m_sound && m_sound_effects) {
    switch (effect) {
      case Chorus:
        m_sound_effects->disableChorusSoundEffect();
        break;
      case Compressor:
        m_sound_effects->disableCompressorSoundEffect();
        break;
      case Distortion:
        m_sound_effects->disableDistortionSoundEffect();
        break;
      case Echo:
        m_sound_effects->disableEchoSoundEffect();
        break;
      case Flanger:
        m_sound_effects->disableFlangerSoundEffect();
        break;
      case Gargle:
        m_sound_effects->disableGargleSoundEffect();
        break;
      case I3DL2Reverb:
        m_sound_effects->disableI3DL2ReverbSoundEffect();
        break;
      case ParamEq:
        m_sound_effects->disableParamEqSoundEffect();
        break;
      case WavesReverb:
        m_sound_effects->disableWavesReverbSoundEffect();
        break;
      default:
        Yeager::Log(ERROR, "Something goes so wrong here! :( invalid sound effect enum!");
    }
  } else {
    Yeager::Log(WARNING,
                "Cannot disable sound effect for AudioHandle name {} UUID {}, no m_sound or m_sound_effect initialized "
                "correctly!",
                mName, uuids::to_string(mEntityUUID));
  }
}

bool AudioHandle::IsSoundEffectEnable(AudioHandleSoundEffects effect)
{
  if (m_sound && m_sound_effects) {
    switch (effect) {
      case Chorus:
        return m_sound_effects->isChorusSoundEffectEnabled();
        break;
      case Compressor:
        return m_sound_effects->isCompressorSoundEffectEnabled();
        break;
      case Distortion:
        return m_sound_effects->isDistortionSoundEffectEnabled();
        break;
      case Echo:
        return m_sound_effects->isEchoSoundEffectEnabled();
        break;
      case Flanger:
        return m_sound_effects->isFlangerSoundEffectEnabled();
        break;
      case Gargle:
        return m_sound_effects->isGargleSoundEffectEnabled();
        break;
      case I3DL2Reverb:
        return m_sound_effects->isI3DL2ReverbSoundEffectEnabled();
        break;
      case ParamEq:
        return m_sound_effects->isParamEqSoundEffectEnabled();
        break;
      case WavesReverb:
        return m_sound_effects->isWavesReverbSoundEffectEnabled();
        break;
      default:
        Yeager::Log(ERROR, "Something goes so wrong here! :( invalid sound effect enum!");
    }
  } else {
    Yeager::Log(WARNING,
                "Cannot check sound effect for AudioHandle name {} UUID {}, no m_sound or m_sound_effect initialized "
                "correctly!",
                mName, uuids::to_string(mEntityUUID));
    return false;
  }
}

bool Yeager::AudioHandle::CheckIfSoundEffectIsSupported()
{
  // TODO:
  return false;
}

AudioHandle::AudioHandle(const EntityBuilder& builder, String path, AudioEngineHandle* handle, bool looped)
    : GameEntity(EntityBuilder(builder.Application, builder.Name, EntityObjectType::AUDIO_HANDLE, builder.UUID)),
      m_EngineHandle(handle),
      m_path(path),
      m_looped(looped)
{
  m_sound_source = m_EngineHandle->Engine->addSoundSourceFromFile(path.c_str(), ESM_AUTO_DETECT, false);
  if (!m_sound_source) {
    Yeager::Log(ERROR, "Cannot Create AudioHandle name {} UUID {}", mName, uuids::to_string(mEntityUUID));
  } else {
    Yeager::Log(INFO, "Create AudioHandle name {} UUID {}", mName, uuids::to_string(mEntityUUID));
  }
}

bool AudioHandle::isPlaying()
{
  if (m_EngineHandle->Engine && m_sound_source) {
    return m_EngineHandle->Engine->isCurrentlyPlaying(m_sound_source);
  }
  return false;
}

void AudioHandle::StopAll()
{
  if (m_EngineHandle->Engine) {
    m_EngineHandle->Engine->stopAllSounds();
  }
}

void AudioHandle::Update()
{
  if (m_EngineHandle->Engine) {
    m_EngineHandle->Engine->update();
  }
}

void AudioHandle::DisableAllSoundEffects()
{
  if (m_sound && m_sound_effects) {
    m_sound_effects->disableAllEffects();
  }
}

AudioHandle::~AudioHandle()
{
  Yeager::Log(INFO, "Destroying AudioHandle name {} UUID {}", mName, uuids::to_string(mEntityUUID));
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
    m_sound = m_EngineHandle->Engine->play2D(m_sound_source, m_looped, false, true, true);
    if (!m_sound) {
      Yeager::Log(ERROR, "Cannot play music! ISound* havent been initialized! Name {} UUID {}", mName,
                  uuids::to_string(mEntityUUID));
    } else {
      SAudioStreamFormat format = m_sound_source->getAudioFormat();
      const char* formatSampleFormat = format.SampleFormat == ESF_U8 ? "ESF_U8" : "ESF_S16";

      Yeager::Log(
          INFO,
          "AudioHandle name {} UUID {}, Info: Sample size {}, Frame size {}, Sample data size {}, Bytes per seconds {} \n \
          Channel count {}, Frame count {}, Sample rate {}, Sample Format {}",
          mName, uuids::to_string(mEntityUUID), format.getSampleSize(), format.getFrameSize(),
          format.getSampleDataSize(), format.getBytesPerSecond(), format.ChannelCount, format.FrameCount,
          format.SampleRate, formatSampleFormat);
      m_stopped = false;
      m_sound_effects = m_sound->getSoundEffectControl();
    }
  }
}

void AudioHandle::Stop()
{
  if (m_sound) {
    m_sound->stop();
    m_stopped = true;
  }
}

void AudioHandle::Pause()
{
  if (!m_paused && m_sound) {
    m_paused_play_pos = m_sound->getPlayPosition();
    m_sound->stop();
    m_paused = true;
  }
}

void AudioHandle::Resume()
{
  if (m_paused && m_EngineHandle->Engine) {
    m_sound = m_EngineHandle->Engine->play2D(m_sound_source, m_looped, false, true, true);
    m_sound->setPlayPosition(m_paused_play_pos);
    m_paused = false;
    m_sound_effects = m_sound->getSoundEffectControl();
  }
}

irrklang::ik_u32 AudioHandle::GetLenght()
{
  if (m_sound) {
    return m_sound->getPlayLength();
  }
  return 0;
}

irrklang::ISoundEngine* AudioHandle::GetEngine()
{
  return m_EngineHandle->Engine;
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
    Yeager::Log(INFO, "AudioHandle name {} UUID {}, had received a {}s time shift", mName,
                uuids::to_string(mEntityUUID), -(time_shift / 1000.0f));
  }
}

void Audio3DHandle::SetAudioPos(irrklang::vec3df pos)
{
  m_audio_pos = pos;
  if (m_sound) {
    m_sound->setPosition(pos);
  }
}

Audio3DHandle::Audio3DHandle(const EntityBuilder& builder, String path, AudioEngineHandle* handle, bool looped,
                             irrklang::vec3df position)
    : AudioHandle(builder, path, handle, looped), m_audio_pos(position)
{
  if (m_sound_source) {
    Yeager::Log(INFO, "Create Audio3DHandle name {} UUID {} Position {} {} {}", mName, uuids::to_string(mEntityUUID),
                position.X, position.Y, position.Z);
  }
  SetEntityType(EntityObjectType::AUDIO_3D_HANDLE);
}

Audio3DHandle::~Audio3DHandle()
{
  Yeager::Log(INFO, "Destroying Audio3DHandle name {} UUID {}", mName, uuids::to_string(mEntityUUID));
}

void Audio3DHandle::Play()
{
  if (m_stopped) {
    m_sound = m_EngineHandle->Engine->play3D(m_sound_source, m_audio_pos, m_looped, false, true, true);
    if (!m_sound) {
      Yeager::Log(ERROR, "Cannot play music! ISound* havent been initialized! Name {} UUID {}", mName,
                  uuids::to_string(mEntityUUID));
    } else {
      SAudioStreamFormat format = m_sound_source->getAudioFormat();
      const char* formatSampleFormat = format.SampleFormat == ESF_U8 ? "ESF_U8" : "ESF_S16";

      Yeager::Log(
          INFO,
          "Audio3DHandle name {} UUID {}, Info: Sample size {}, Frame size {}, Sample data size {}, Bytes per seconds {} \n \
          Channel count {}, Frame count {}, Sample rate {}, Sample Format {}",
          mName, uuids::to_string(mEntityUUID), format.getSampleSize(), format.getFrameSize(),
          format.getSampleDataSize(), format.getBytesPerSecond(), format.ChannelCount, format.FrameCount,
          format.SampleRate, formatSampleFormat);
      m_stopped = false;
      m_sound_effects = m_sound->getSoundEffectControl();
    }
  }
}
void Audio3DHandle::Resume()
{
  if (m_paused && m_EngineHandle->Engine) {
    m_sound = m_EngineHandle->Engine->play3D(m_sound_source, m_audio_pos, m_looped, false, true, true);
    m_sound->setPlayPosition(m_paused_play_pos);
    m_paused = false;
    m_sound_effects = m_sound->getSoundEffectControl();
  }
}
irrklang::vec3df Audio3DHandle::GetIrrklangPosition()
{
  if (m_sound) {
    return m_sound->getPosition();
  } else {
    return irrklang::vec3df(0.0f, 0.0f, 0.0f);
  }
}

Vector3 Audio3DHandle::GetVector3Position()
{
  if (m_sound) {
    return Vec3dfToGLMVec3(m_sound->getPosition());
  } else {
    return Vector3(0.0f);
  }
}

Vector3 Yeager::Vec3dfToGLMVec3(irrklang::vec3df vec)
{
  Vector3 vector;
  vector.x = vec.X;
  vector.y = vec.Y;
  vector.z = vec.Z;
  return vector;
}

irrklang::vec3df Yeager::GLMVec3ToVec3df(Vector3 vec)
{
  irrklang::vec3df vector;
  vector.X = vec.x;
  vector.Y = vec.y;
  vector.Z = vec.z;
  return vector;
}
