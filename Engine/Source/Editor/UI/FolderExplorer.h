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
#include "Common/Utils/LogEngine.h"
#include "Common/Utils/Utilities.h"

#include "Components/Renderer/Texture/TextureHandle.h"
#include "UIManagement.h"

#define YEAGER_EMPTY_PATH ""

namespace Yeager {

class ApplicationCore;

/**
 * @brief Represents the type of the directory hierarchy item, if it is a file or folder, even unknown
 */
struct DirectoryHierarchyType {
  enum Enum { eFOLDER, eFILE, eUNKNOWN };
};

/**
 * @brief Holds information about the file, like size, atrributes, creation time, last written and acessed.
 */
struct FileInformation {
  FileInformation() = default;
  unsigned long mAttributes = 0x00;
  uint64_t mFileSize = 0x00;
  Yeager::TimePointType mCreationTime;
  Yeager::TimePointType mLastAcessed;
  Yeager::TimePointType mLastWritten;

  static FileInformation GetFileInformation(const std::filesystem::path& path);

#if defined(YEAGER_SYSTEM_WINDOWS_x64)
  static FileInformation GetWindowsFileInformation(const std::filesystem::path& path);
#elif defined(YEAGER_SYSTEM_LINUX)
  static FileInformation GetLinuxFileInformation(const std::filesystem::path& path);
#endif
};

/**
 * @brief Represents a item that lives in the directory hierarchy of the system, like a folder or file
 */
struct DirectoryHierarchyItem {
  std::filesystem::path mPath;
  std::filesystem::path mParent;
  DirectoryHierarchyType::Enum mType = DirectoryHierarchyType::eUNKNOWN;
  String mName = YEAGER_NULL_LITERAL;
  bool bIsSelected = false;
  bool bSearchAppears = true;
  FileInformation mInfo;
};

class FolderExplorer {
 public:
  typedef std::filesystem::path Path;

  FolderExplorer(Yeager::ApplicationCore* application, const Path& start = std::filesystem::current_path());
  FolderExplorer() = default;

  void DrawWindow();
  void LoadIcons();

  YEAGER_NODISCARD static std::vector<DirectoryHierarchyItem> GetItemsFromFolder(const Path& path);
  YEAGER_NODISCARD static std::vector<DirectoryHierarchyItem> GetItemsFromFolder(const DirectoryHierarchyItem& item);

  Path GetCurrentPath() const;
  Path GetLastPath() const;
  Path GetLastKnownWorkingPath() const;
  Path GoBackLastKnownPath() const;

  std::vector<DirectoryHierarchyItem> GetCurrentItems() const;

 private:
  Path mCurrentPath;
  Path mLastPath;
  Path mLastKnownWorkingPath;
  String mSearchPattern = YEAGER_EMPTY_LITERAL;
  Yeager::ApplicationCore* mApplication = YEAGER_NULLPTR;
  std::vector<DirectoryHierarchyItem> mCurrentItems;
  std::map<DirectoryHierarchyType::Enum, std::shared_ptr<MaterialTexture2D>> mIcons;
};
}  // namespace Yeager