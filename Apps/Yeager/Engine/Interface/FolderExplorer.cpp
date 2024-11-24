#include "FolderExplorer.h"
#include "../../Application.h"
using namespace Yeager;

FolderExplorer::FolderExplorer(Yeager::ApplicationCore* application, const Path& start)
    : mApplication(application), mCurrentPath(start), mLastKnownWorkingPath(start), mLastPath(start)
{
  mCurrentItems = GetItemsFromFolder(start);
  LoadIcons();
}

void FolderExplorer::LoadIcons()
{
  auto folder =
      std::make_shared<MaterialTexture2D>(EntityBuilder(mApplication, "Folder_Icon"), MaterialTextureType::eDIFFUSE);
  folder->GenerateFromFile(GetPathFromShared("/Resources/Icons/folder.png").value(), true);
  mIcons.insert(std::pair<DirectoryHierarchyType::Enum, std::shared_ptr<MaterialTexture2D>>(
      DirectoryHierarchyType::eFOLDER, folder));
  auto file =
      std::make_shared<MaterialTexture2D>(EntityBuilder(mApplication, "File_Icon"), MaterialTextureType::eDIFFUSE);
  file->GenerateFromFile(GetPathFromShared("/Resources/Icons/file.png").value(), true);
  mIcons.insert(
      std::pair<DirectoryHierarchyType::Enum, std::shared_ptr<MaterialTexture2D>>(DirectoryHierarchyType::eFILE, file));
}

FileInformation FileInformation::GetFileInformation(const std::filesystem::path& path)
{
#if defined(YEAGER_SYSTEM_WINDOWS_x64)
  return GetWindowsFileInformation(path);
#elif defined(YEAGER_SYSTEM_LINUX)
  return GetLinuxFileInformation(path);
#endif
}

#if defined(YEAGER_SYSTEM_WINDOWS_x64)

FileInformation FileInformation::GetWindowsFileInformation(const std::filesystem::path& path)
{
  WIN32_FILE_ATTRIBUTE_DATA info = {0};
  GetFileAttributesEx(path.string().c_str(), GetFileExInfoStandard, &info);
  FileInformation file;
  file.mAttributes = info.dwFileAttributes;
  file.mCreationTime = WindowsFILETIMEToTimePoint(info.ftCreationTime);
  file.mLastAcessed = WindowsFILETIMEToTimePoint(info.ftLastAccessTime);
  file.mLastWritten = WindowsFILETIMEToTimePoint(info.ftLastWriteTime);
  file.mFileSize = Convert32BitsTo64Bits(info.nFileSizeHigh, info.nFileSizeLow);
  return file;
}

#elif defined(YEAGER_SYSTEM_LINUX)
FileInformation FileInformation::GetLinuxFileInformation(const std::filesystem::path& path)
{
  return FileInformation();
}
#endif

void FolderExplorer::DrawWindow()
{
  ImGui::Begin("File Explorer", NULL, YEAGER_WINDOW_MOVEABLE);

  if (mLastPath != mCurrentPath) {
    try {
      mCurrentItems = GetItemsFromFolder(mCurrentPath);
    } catch (std::filesystem::filesystem_error& err) {
      Yeager::Log(ERROR, "Cannot access Folder: {}", err.what());
      mCurrentPath = mLastPath;
    }
    mLastPath = mCurrentPath;
  }

  if (ImGui::InputText("Search:", &mSearchPattern)) {
    if (!mSearchPattern.empty() && mSearchPattern != YEAGER_EMPTY_LITERAL) {
      for (auto& item : mCurrentItems) {
        (!BruteForceStringSearch(ToLower(mSearchPattern), ToLower(item.mName)).empty()) ? item.bSearchAppears = true
                                                                                       : item.bSearchAppears = false;
      }
    } else {
      for (auto& item : mCurrentItems) {
        item.bSearchAppears = true;
      }
    }
  }

  if (ImGui::Button("Back")) {
    if (mCurrentPath.has_parent_path()) {
      mCurrentPath = mCurrentPath.parent_path();
    }
  }

  ImGui::SameLine();
  if (ImGui::Button("Refresh")) {
    mCurrentItems = GetItemsFromFolder(mCurrentPath);
  }

  ImGui::Text("Current path: %s", mCurrentPath.string().c_str());

  ImGui::BeginChild("##item");

  for (const auto& item : mCurrentItems) {
    if (!item.bSearchAppears)
      continue;

    String type = (item.mType == DirectoryHierarchyType::eFOLDER ? "Folder" : "File");
    String symbol = (item.mType == DirectoryHierarchyType::eFOLDER ? ICON_FA_FOLDER : ICON_FA_FILE);
    String name = String(symbol + " [" + type + "] " + item.mName);

    if (ImGui::Selectable(name.c_str(), false, ImGuiSelectableFlags_AllowDoubleClick)) {
      if (item.mType == DirectoryHierarchyType::eFOLDER) {
        mCurrentPath = std::filesystem::path(mCurrentPath / item.mName);
      } else {
        const auto filetype = CheckFileExtensionType(item.mPath, FileExtensionType::EExtensitonType3DModel);
        if (filetype.first && filetype.second.Supported) {
          mApplication->GetExplorer()->QuickLoadObject(item.mPath);
        }
      }
    }
  }
  ImGui::EndChild();

  ImGui::End();
}
std::vector<DirectoryHierarchyItem> FolderExplorer::GetItemsFromFolder(const Path& path)
{
  if (!std::filesystem::exists(path) || std::filesystem::is_regular_file(path))
    return std::vector<DirectoryHierarchyItem>{};  // Dont exists or is a file!

  std::vector<DirectoryHierarchyItem> entries;

  for (const auto& item : std::filesystem::directory_iterator(path)) {
    DirectoryHierarchyItem entry;
    entry.mType =
        (std::filesystem::is_directory(item) ? DirectoryHierarchyType::eFOLDER : DirectoryHierarchyType::eFILE);
    entry.mName = (item.path().has_filename() ? item.path().filename().string() : YEAGER_NULL_LITERAL);
    entry.mPath = item.path();
    entry.mParent = (item.path().has_parent_path() ? item.path().parent_path() : YEAGER_EMPTY_PATH);
    entry.mInfo = (entry.mType == DirectoryHierarchyType::eFILE ? FileInformation::GetFileInformation(entry.mPath)
                                                              : FileInformation());
    entries.push_back(entry);
  }
  return entries;
}

std::vector<DirectoryHierarchyItem> FolderExplorer::GetItemsFromFolder(const DirectoryHierarchyItem& item)
{
  if (item.mType == DirectoryHierarchyType::eFOLDER)
    return GetItemsFromFolder(item.mPath);
  return std::vector<DirectoryHierarchyItem>{};  // Is a file!
}

std::filesystem::path FolderExplorer::GetCurrentPath() const
{
  return mCurrentPath;
}

std::filesystem::path FolderExplorer::GetLastKnownWorkingPath() const
{
  return mLastKnownWorkingPath;
}

std::filesystem::path FolderExplorer::GoBackLastKnownPath() const
{
  return (std::filesystem::exists(mLastKnownWorkingPath) ? mLastKnownWorkingPath : std::filesystem::current_path());
}

std::filesystem::path FolderExplorer::GetLastPath() const
{
  return mLastPath;
}

std::vector<DirectoryHierarchyItem> FolderExplorer::GetCurrentItems() const
{
  return mCurrentItems;
}