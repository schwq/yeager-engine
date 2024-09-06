#include "FolderExplorer.h"
#include "../../Application.h"
using namespace Yeager;

FolderExplorer::FolderExplorer(Yeager::ApplicationCore* application, const std::filesystem::path& start)
    : m_Application(application), m_CurrentPath(start), m_LastKnownWorkingPath(start), m_LastPath(start)
{
  m_CurrentItems = GetItemsFromFolder(start);
  LoadIcons();
}

void FolderExplorer::LoadIcons()
{
  auto folder = std::make_shared<MaterialTexture2D>(m_Application, "Folder_Icon", MaterialTextureType::eDIFFUSE);
  folder->GenerateFromFile(GetPathFromShared("/Resources/Icons/folder.png").value(), true);
  m_Icons.insert(std::pair<DirectoryHierarchyType::Enum, std::shared_ptr<MaterialTexture2D>>(DirectoryHierarchyType::eFOLDER, folder));
  auto file = std::make_shared<MaterialTexture2D>(m_Application, "File_Icon", MaterialTextureType::eDIFFUSE);
  file->GenerateFromFile(GetPathFromShared("/Resources/Icons/file.png").value(), true);
  m_Icons.insert(std::pair<DirectoryHierarchyType::Enum, std::shared_ptr<MaterialTexture2D>>(DirectoryHierarchyType::eFILE, file));
}

FileInformation FileInformation::GetFileInformation(const std::filesystem::path& path) {
#if defined(YEAGER_SYSTEM_WINDOWS_x64)
  return GetWindowsFileInformation(path);
#elif defined(YEAGER_SYSTEM_LINUX)
  return GetLinuxFileInformation(path);
#endif
}

#if defined(YEAGER_SYSTEM_WINDOWS_x64)

FileInformation FileInformation::GetWindowsFileInformation(const std::filesystem::path& path) {
  WIN32_FILE_ATTRIBUTE_DATA info = {0};
  GetFileAttributesEx(path.string().c_str(), GetFileExInfoStandard, &info);
  FileInformation file;
  file.Attributes = info.dwFileAttributes;
  file.CreationTime = WindowsFILETIMEToTimePoint(info.ftCreationTime);
  file.LastAcessed = WindowsFILETIMEToTimePoint(info.ftLastAccessTime);
  file.LastWritten = WindowsFILETIMEToTimePoint(info.ftLastWriteTime);
  file.FileSize = Convert32BitsTo64Bits(info.nFileSizeHigh, info.nFileSizeLow);
  return file;
}

#elif defined(YEAGER_SYSTEM_LINUX)
FileInformation FileInformation::GetLinuxFileInformation(const std::filesystem::path& path);
#endif

void FolderExplorer::DrawWindow()
{
  ImGui::Begin("File Explorer", NULL, YEAGER_WINDOW_MOVEABLE);

  if (m_LastPath != m_CurrentPath) {
    try {
      m_CurrentItems = GetItemsFromFolder(m_CurrentPath);
    }
    catch (std::filesystem::filesystem_error& err) {
      Yeager::Log(ERROR, "Cannot access Folder: {}", err.what());
      m_CurrentPath = m_LastPath;
    }
    m_LastPath = m_CurrentPath;
  }

  if (ImGui::Button("Back")) {
      if (m_CurrentPath.has_parent_path()) {
      m_CurrentPath = m_CurrentPath.parent_path();
    }
  }

  ImGui::SameLine();
  if (ImGui::Button("Refresh")) {
    m_CurrentItems = GetItemsFromFolder(m_CurrentPath);
  }

  ImGui::Text("Current path: %s", m_CurrentPath.string().c_str());

  ImGui::BeginChild("##item");

  for (const auto& item : m_CurrentItems) {
    String type = (item.Type == DirectoryHierarchyType::eFOLDER ? "Folder" : "File"); 
    String symbol = (item.Type == DirectoryHierarchyType::eFOLDER ? ICON_FA_FOLDER : ICON_FA_FILE);
    String name = String(symbol + " [" + type + "] " + item.Name);
    
    if (ImGui::Selectable(name.c_str(), false, ImGuiSelectableFlags_AllowDoubleClick)) {
      if (item.Type == DirectoryHierarchyType::eFOLDER) {
        m_CurrentPath = std::filesystem::path(m_CurrentPath / item.Name);
      } else {
        const auto filetype = CheckFileExtensionType(item.Path, FileExtensionType::EExtensitonType3DModel);
        if (filetype.first && filetype.second.Supported) {
          m_Application->GetExplorer()->QuickLoadObject(item.Path);
        }
      }
    }
  }
  ImGui::EndChild();

  ImGui::End();

}
std::vector<DirectoryHierarchyItem> FolderExplorer::GetItemsFromFolder(const std::filesystem::path& path) {
  if (!std::filesystem::exists(path) || std::filesystem::is_regular_file(path))
    return std::vector<DirectoryHierarchyItem>{};  // Dont exists or is a file!

  std::vector<DirectoryHierarchyItem> entries;

  for (const auto& item : std::filesystem::directory_iterator(path)) {
    DirectoryHierarchyItem entry;
    entry.Type = (std::filesystem::is_directory(item) ? DirectoryHierarchyType::eFOLDER
                                        : DirectoryHierarchyType::eFILE);
    entry.Name = (item.path().has_filename() ? item.path().filename().string() : YEAGER_NULL_LITERAL);
    entry.Path = item.path();
    entry.Parent = (item.path().has_parent_path() ? item.path().parent_path() : YEAGER_EMPTY_PATH);
    entry.Info = (entry.Type == DirectoryHierarchyType::eFILE ? FileInformation::GetFileInformation(entry.Path)
                                                              : FileInformation());
    entries.push_back(entry);
  }
  return entries;
}

std::vector<DirectoryHierarchyItem> FolderExplorer::GetItemsFromFolder(const DirectoryHierarchyItem& item) {
  if (item.Type == DirectoryHierarchyType::eFOLDER)
    return GetItemsFromFolder(item.Path);
  return std::vector<DirectoryHierarchyItem>{}; // Is a file!
}

std::filesystem::path FolderExplorer::GetCurrentPath() const {
  return m_CurrentPath;
}

std::filesystem::path FolderExplorer::GetLastKnownWorkingPath() const {
    return m_LastKnownWorkingPath;
}

std::filesystem::path FolderExplorer::GoBackLastKnownPath() const
{
  return (std::filesystem::exists(m_LastKnownWorkingPath) ? m_LastKnownWorkingPath : std::filesystem::current_path());
}


std::filesystem::path FolderExplorer::GetLastPath() const {
  return m_LastPath;
}

std::vector<DirectoryHierarchyItem> FolderExplorer::GetCurrentItems() const {
  return m_CurrentItems;
}