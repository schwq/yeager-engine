#include "DirectorySystem.h"
#include "Common/Utils/Utilities.h"
using namespace Yeager;

std::map<String, FileType> Yeager::g_ExtensionTypesToRawExtensions = {
    {"ERROR", FileType("ERROR_NOT_FILETYPE", EExtensionTypeSource, false)},
    {".obj", FileType("3D Model Object", EExtensitonType3DModel, true)},
    {".fbx", FileType("3D Model Object", EExtensitonType3DModel, true)},
    {".dae", FileType("3D Model Object", EExtensitonType3DModel, true)},
    {".blend", FileType("3D Model Blender Object", EExtensitonType3DModel)},
    {".yml", FileType("Configuration Serialization Data", EExtensionTypeConfiguration)},
    {".yaml", FileType("Configuration Serialization Data", EExtensionTypeConfiguration)},
    {".wav", FileType("Audio File", EExtensionTypeAudio, true)},
    {".mp3", FileType("Audio File", EExtensionTypeAudio, true)},
    {".cpp", FileType("C++ Source File", EExtensionTypeSource, false)},
    {".cc", FileType("C++ Source File", EExtensionTypeSource, false)},
    {".cxx", FileType("C++ Source File", EExtensionTypeSource, false)},
    {".c", FileType("C Source File", EExtensionTypeSource, false)},
    {".h", FileType("C Header File", EExtensionTypeSource, false)},
    {".hpp", FileType("C++ Header File", EExtensionTypeSource, false)},
    {".hxx", FileType("C++ Header File", EExtensionTypeSource, false)},
    {".hh", FileType("C++ Header File", EExtensionTypeSource, false)}};

FileType::FileType(String type, FileExtensionType ext, bool supported)
{
  Typename = type;
  ExtType = ext;
  Supported = supported;
}

bool Yeager::CreateDirectoryAndValidate(const std::filesystem::path& p)
{
  if (!std::filesystem::create_directory(p)) {
    Yeager::Log(ERROR, "Cannot create directory in path: %s", p.string());
    return false;
  }
  return true;
}
bool Yeager::ValidatesAndCreateDirectory(const std::filesystem::path& p)
{
  bool exists = Yeager::ValidatesPath(p);
  bool dir = true;
  if (!exists) {
    dir = Yeager::CreateDirectoryAndValidate(p);
  }
  return exists || dir;
}

String Yeager::ExtractExtensionTypenameFromPath(const std::filesystem::path& path)
{
  return g_ExtensionTypesToRawExtensions[ExtractExtensionFromFilename(path.string())].Typename;
}

std::pair<bool, FileType> Yeager::CheckFileExtensionType(const std::filesystem::path& path, FileExtensionType type)
{
  const String ext = ExtractExtensionFromFilename(path.string());
  if (auto search = g_ExtensionTypesToRawExtensions.find(ext); search != g_ExtensionTypesToRawExtensions.end()) {
    return std::pair<bool, FileType>(search->second.ExtType == type, search->second);
  }
  return std::pair<bool, FileType>(false, g_ExtensionTypesToRawExtensions["ERROR"]);
}

String Yeager::RemoveExtensionFromFilename(String filename)
{
  if (filename.empty())
    return String(YEAGER_STRING_ERROR("RemoveExtensionFromFilename, String empty"));

  size_t pos = filename.find_last_of('.');
  if (pos == String::npos) {
    Yeager::Log(ERROR, "Remove extension from filename {}, . character cannot be found in the expression!", filename);
    return String(YEAGER_STRING_ERROR("Character not found!"));
  }
  return filename.substr(0, pos);
}

String Yeager::ExtractExtensionFromFilename(String filename)
{
  if (filename.empty())
    return String(YEAGER_STRING_ERROR("ExtractExtensionFromFilename, String empty"));

  size_t pos = filename.find_last_of('.');
  if (pos == String::npos) {
    Yeager::Log(ERROR, "Extract extension from filename {}, . character cannot be found in the expression!", filename);
    return String(YEAGER_STRING_ERROR("Character not found!"));
  }
  return filename.substr(pos, filename.length());
}

size_t Yeager::NumberOfFilesInDir(const std::filesystem::path& p)
{
  using std::filesystem::directory_iterator;
  return std::distance(directory_iterator(p), directory_iterator{});
}

String Yeager::FileContentToString(const std::filesystem::path& p)
{
  if (!ValidatesPath(p)) {
    return String(YEAGER_STRING_ERROR("File doesnt exists!"));
  }
  std::ifstream in;
  String s;
  String result;
  in.open(p.string().c_str());
  while (!in.eof()) {
    getline(in, s);
    result += s + '\n';
  }
  in.close();
  return result;
}

bool Yeager::ValidatesPath(const std::filesystem::path& p, bool declare, std::filesystem::file_status s)
{
  bool known = std::filesystem::status_known(s) ? std::filesystem::exists(s) : std::filesystem::exists(p);
  String status = known ? "Validated!" : "Not Validated!";
#ifdef YEAGER_DEBUG
  if (declare)
  //Yeager::Log((status == "Validated!" ? INFO : ERROR), "File validation: Path: {}, Status {}", p.c_str(), status);
#endif
    return known;
}

String Yeager::GetExternalLocalFolderPath()
{
#ifdef YEAGER_SYSTEM_LINUX
  const char* homeDir;
  /* The $HOME env varaible does not exist */
  if ((homeDir = getenv("HOME")) == NULL) {
    homeDir = getpwuid(getuid())->pw_dir;
    return String(homeDir) + String("/.local/share");
  } else {
    /* The $HOME env variable exists!*/
    return String(getenv("HOME")) + String("/.local/share");
  }
#elif defined(YEAGER_SYSTEM_WINDOWS_x64)
  return GetWindowsAppDataFolder();
#endif
}

String Yeager::GetExternalSharedFolderPath()
{
#ifdef YEAGER_SYSTEM_LINUX
  return String("/usr/share");
#elif defined(YEAGER_SYSTEM_WINDOWS_x64)
  return GetWindowsProgramFilesFolder();
#endif
}

#if defined(YEAGER_SYSTEM_WINDOWS_x64)

String Yeager::GetWindowsAppDataFolder()
{
  TCHAR path[MAX_PATH];
  if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, path))) {
    return String(path);
  }
  Yeager::LogDebug(ERROR, "SHGetFolderPathA cannot find the APPDATA folder!");
  return String(YEAGER_NULL_LITERAL);
}

String Yeager::GetWindowsProgramFilesFolder()
{
  TCHAR path[MAX_PATH];
  if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_PROGRAM_FILES, NULL, 0, path))) {
    return String(path);
  }
  Yeager::LogDebug(ERROR, "SHGetFolderPathA cannot find the PROGRAMFILES folder!");
  return String(YEAGER_NULL_LITERAL);
}

#endif

std::vector<String> Yeager::RetriveSubdirectoriesFromPath(const std::filesystem::path& p)
{
  return RetrievePhrasesBetweenChar(p.string(), YG_PS);
}

std::filesystem::path Yeager::FindCommonDirectoryFromPaths(const std::filesystem::path& p1,
                                                           const std::filesystem::path& p2)
{
  if (p1 == p2)
    return p1;

  std::vector<String> sp1 = RetriveSubdirectoriesFromPath(p1);
  std::vector<String> sp2 = RetriveSubdirectoriesFromPath(p2);
  String common;

  for (size_t index = 0; index < sp1.size(); index++) {
    if (sp1.at(index) == sp2.at(index)) {
      common += sp1.at(index) + YG_PS;
    } else {
      return std::filesystem::path(common);
    }
  }
  return std::filesystem::path(common);
}

String Yeager::ConvertNearestFileSizeName(const uint64_t bytes)
{

  if (bytes < 1024)  // Less that a KB!
    return std::to_string(bytes) + " Bytes";

  if (bytes >= 1024 && bytes < (1024 * 1024))  // Between 1KB and 1MB
    return std::to_string((float)bytes / 1024) + "KB";

  if (bytes >= (1024 * 1024) && bytes < (1024 * 1024 * 1024))  // Between 1MB and 1GB
    return std::to_string((float)bytes / (1024 * 1024)) + "MB";

  return std::to_string((float)bytes / (1024 * 1024 * 1024)) + "GB";
}

std::tuple<std::filesystem::path, std::vector<String>, std::vector<String>>
Yeager::FindCommonDirectoryFromPathsAndDifference(const std::filesystem::path& p1, const std::filesystem::path& p2)
{
  if (p1 == p2)
    return std::make_tuple(p1, std::vector<String>{}, std::vector<String>{});

  std::vector<String> sp1 = RetriveSubdirectoriesFromPath(p1);
  std::vector<String> sp2 = RetriveSubdirectoriesFromPath(p2);
  String common;
  size_t index;
  for (index = 0; index < sp1.size(); index++) {
    if (sp1.at(index) == sp2.at(index)) {
      common += sp1.at(index) + YG_PS;
    } else {
      break;
    }
  }
  sp1.erase(sp1.begin(), sp1.begin() + index);
  sp2.erase(sp2.begin(), sp2.begin() + index);
  return std::make_tuple(std::filesystem::path(common), sp1, sp2);
}

std::optional<String> Yeager::GetPathFromLocal(const String& path)
{
  String p = String("/YeagerEngine" + path);
  if (g_OperatingSystemString == YEAGER_WINDOWS32_OS_STRING)
    std::replace(p.begin(), p.end(), '/', '\\');
  String f = String(GetExternalLocalFolderPath() + p);
  if (Yeager::ValidatesPath(f))
    return f;
  Yeager::LogDebug(WARNING, "GetPathFromLocal optional does not returns a value! Path: {}", f);
  return std::nullopt;
}

std::optional<String> Yeager::GetPathFromShared(const String& path)
{
  String p = String("/YeagerEngine" + path);
  if (g_OperatingSystemString == YEAGER_WINDOWS32_OS_STRING)
    std::replace(p.begin(), p.end(), '/', '\\');
  String f = String(GetExternalSharedFolderPath() + p);
  if (Yeager::ValidatesPath(f))
    return f;
  Yeager::LogDebug(WARNING, "GetPathFromLocal optional does not returns a value! Path: {}", f);
  return std::nullopt;
}

String Yeager::GetPathFromSourceCode(String path)
{
  if (g_OperatingSystemString == YEAGER_WINDOWS32_OS_STRING)
    std::replace(path.begin(), path.end(), '/', '\\');
  return String(std::filesystem::current_path().string() + YG_PS + "Engine" + YG_PS + "Source" + path);
}

void Yeager::DumpConsoleDataInFile(const std::filesystem::path& path)
{
  std::ofstream out(path);
  std::vector<String> data;
  if (out.is_open()) {
    for (const auto& m : *gGlobalConsole.GetLogs()) {
      data.push_back(m.message);
    }
    std::ostream_iterator<String> it(out, "\n");
    std::copy(std::begin(data), std::end(data), it);
  }
  out.close();
}