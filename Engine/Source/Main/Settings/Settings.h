// Yeager Engine, free and open source 3D / 2D renderer written in OpenGL
//    In case of questions and bugs, please, refer to the issue tab on github
//    Repo : https://github.com/schwq/YeagerEngine

//    Copyright (C) 2023 - Present
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.e.h
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include "Common/Utils/Common.h"
#include "Common/Utils/LogEngine.h"
#include "Common/Utils/Utilities.h"

#include "Main/Window/Window.h"

namespace Yeager {
class ApplicationCore;

struct AntiAliasingOption {
  enum Enum { MSAA_x2, MSAA_x4 };
  static Enum toEnum(String str);
  static String toString(Enum type);
};

struct EngineConfigurationHandle {
  ImVec2 LauncherWindowSize = ImVec2(800, 800);
  ImVec2 EditorWindowSize = ImVec2(1920, 1080);
};

struct VideoSettings {
  AntiAliasingOption::Enum AntiAliasingType;
  bool WindowObeysAspectRatio = true;
  AspectRatio::Enum WindowDesireAspectRatio = AspectRatio::eASPECT_21_9;
};

struct InterfaceSettings {
  float GlobalOnScreenTextScale = 0.5f;
};

class Settings {
 public:
  Settings(Yeager::ApplicationCore* application);
  ~Settings();
  YEAGER_FORCE_INLINE VideoSettings GetVideoSettingsStruct() const { return m_VideoSettings; }
  YEAGER_FORCE_INLINE VideoSettings* GetVideoSettingsStructPtr() { return &m_VideoSettings; }
  YEAGER_FORCE_INLINE InterfaceSettings GetInterfaceSettingsStruct() const { return m_InterfaceSettings; }
  YEAGER_FORCE_INLINE InterfaceSettings* GetInterfaceSettingsStructPtr() { return &m_InterfaceSettings; }
  YEAGER_FORCE_INLINE EngineConfigurationHandle* GetEngineConfiguration() { return &m_EngineConfiguration; }
  /* Use this function to change values of the settings, it verify everything to make it happen correctly*/
  bool ChangeVideoSettingsAntiAliasingType(AntiAliasingOption::Enum type);

 private:
  void InitializeDefaultValues();

  Yeager::ApplicationCore* m_Application = YEAGER_NULLPTR;
  VideoSettings m_VideoSettings;
  InterfaceSettings m_InterfaceSettings;
  EngineConfigurationHandle m_EngineConfiguration;
};
}  // namespace Yeager
