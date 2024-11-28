//    Yeager Engine, free and open source 3D/2D renderer written in OpenGL
//    In case of questions and bugs, please, refer to the issue tab on github
//    Repo : https://github.com/schwq/YeagerEngine
//
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
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include "Common/Utils/Common.h"
#include "Common/Utils/Utilities.h"

namespace Yeager {

#define YEAGER_TRANSLATE_ERROR_STRING "TRANSLATE_ERROR"
#define YEAGER_NO_LOCALE_LOADED_STRING "LOCALE_NOT_LOADED"

class ApplicationCore;

struct ELanguangeRegion {
  enum Enum { EN_US, PT_BR, Unknown };
  YEAGER_ENUM_TO_STRING(ELanguangeRegion)
  YEAGER_STRING_TO_ENUM(ELanguangeRegion)
};

/**
 * TODO: Make this functionality so that multiple locale files and be stored in a single Locale class
 * The current system is all stored in a file with the prefix os Yeager_(languange region)
 * In the future, multiple files, like Launcher_(languange region), Editor(languange region) are gonna to exist
 */
extern std::optional<std::filesystem::path> GetLocalePathFromLanguangeRegion(const ELanguangeRegion::Enum& language);

class Locale {
 public:
  Locale() = default;
  Locale(Yeager::ApplicationCore* application, const ELanguangeRegion::Enum& language, bool s_assert = true);

  bool Load(const std::filesystem::path& path);
  String Translate(String key) const;

  bool IsLoaded() const { return bLoaded; }
  std::filesystem::path GetPath() const { return mLocalePath; }
  std::map<String, String> GetData() const { return mMapData; }
  ELanguangeRegion::Enum GetLanguageRegion() const { return mLanguageRegion; }

 private:
  std::filesystem::path mLocalePath;
  std::map<String, String> mMapData;
  ELanguangeRegion::Enum mLanguageRegion = ELanguangeRegion::Unknown;
  bool bLoaded = false;
  Yeager::ApplicationCore* mApplication = YEAGER_NULLPTR;
};
}  // namespace Yeager
