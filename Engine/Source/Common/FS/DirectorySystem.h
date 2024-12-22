//    Yeager Engine, free and open source 3D/2D renderer written in OpenGL
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
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include "Common/Utils/Common.h"
#include "Common/Utils/LogEngine.h"
#include "FileUtils.h"

namespace Yeager {

/* 
    Author comment: When writing code that works with folders, files, paths, ect, feel free to use Strings of std::filesystem::path to represents paths.
    Even so, please give preference to std::filesystem::path that works the best when validating and joining paths separators!
*/

/**
    @brief Represents the type of extensions from a file, for example, the extension .obj and .dae represents a 3D model object file,
    like .mp3 and .wav that both represents a audio file.
*/
enum FileExtensionType {
  EExtensitonType3DModel,
  EExtensionTypeAudio,
  EExtensionTypeImage,
  EExtensionTypeVideo,
  EExtensionTypeSource,
  EExtensionTypeConfiguration
};

/**
    @brief Represents a file information that is important to the engine to know, like the type, the kind of extension and if is supported
    by the engine or not. It is great because it can be used to call different parts of the engine for different files 
*/
struct FileType {
  FileType() = default;
  FileType(String type, FileExtensionType ext, bool supported = false);
  String Typename = YEAGER_NULL_LITERAL;
  FileExtensionType ExtType = FileExtensionType::EExtensionTypeSource;
  bool Supported = false;
};

/**
    @brief A collection of raw extensions and the file type associated with it, for example: the raw extension ".obj" is associated in the map
    with the file type with type "3D model Object", extension type EExtensitonType3DModel and is supported by the engine. This is a global variable
    so it is marked with the g_ prefix
*/
extern std::map<String, FileType> g_ExtensionTypesToRawExtensions;

/**
    @brief Checks if the extension of the path is list among the ExtensionTypesToRawExtensions, returns if true. This doesnt check if the 
    extensions is  supported by the engine. This function returns a ERROR type if not found!
*/
YEAGER_NODISCARD std::pair<bool, FileType> CheckFileExtensionType(const std::filesystem::path& path,
                                                                  FileExtensionType type);

/**
    @brief Returns the extensions typename from a given filename. Example the filename: model.obj, returns "3D Model Object"
*/
YEAGER_NODISCARD extern String ExtractExtensionTypenameFromPath(const std::filesystem::path& path);

/**
    @brief Given a path, returns the absolute path relative to the engine "source code folder". This function is kinda problematic because 
    the source code folder path for the engine is relative to std::filesystem::current_path() which can be called from a binary outside the actual
    folder and returns a path that not exists!
    
    @attention Read the above! Dont use!
*/
YEAGER_NODISCARD extern String GetPathFromSourceCode(String path);

/** 
    @brief Returns the local folder path of the engine, for Windows systems is the %appdata% folder, 
    and for Linux systems is the $HOME/.local/share folder 
*/
YEAGER_NODISCARD extern String GetExternalLocalFolderPath();

/** 
    @brief Returns the shared folder path, for Windows systems is the (Drive Letter):\Program Files folder and 
    for Linux systems is the /usr/share folder 
*/
YEAGER_NODISCARD extern String GetExternalSharedFolderPath();

/** 
    @brief Checks if the directory already exists, if not, it creates it 
*/
extern bool ValidatesAndCreateDirectory(const std::filesystem::path& p);

/** 
    @brief Creates a directory and thrown an error if cannot creates it 
*/
extern bool CreateDirectoryAndValidate(const std::filesystem::path& p);

/**
    @brief Returns the validation of a given path, it also can declare the validation logging it to the engine!
*/
extern bool ValidatesPath(const std::filesystem::path& p, bool declare = true,
                          std::filesystem::file_status s = std::filesystem::file_status());

#if defined(YEAGER_SYSTEM_WINDOWS_x64)

/**
    @brief Returns the %appdata% folder of Windows systems, this is where the public and read and write files and folder of the engine lives in
*/
YEAGER_NODISCARD extern String GetWindowsAppDataFolder();

/**
    @brief Returns the ProgramFiles folder of Windows system, this is where private and readonly files and folders of the engine lives in
*/
YEAGER_NODISCARD extern String GetWindowsProgramFilesFolder();

#endif  // YEAGER_SYSTEM_WINDOWS_x64

/**
    @brief Converts the given number of bytes to a string with the nearest file size name, for example, by passing 5000, the nearest 
    file size name is KB, so it returns ~ 4,8KB (5000 / 1024). Passing 2.000.000 bytes, it returns ~ 1,8MB (2.000.000 / 1024 / 1024) 
*/
YEAGER_NODISCARD extern String ConvertNearestFileSizeName(const uint64_t bytes);

/** 
    @brief Given a path, it convert all the text (content) inside the file to a string 
*/
YEAGER_NODISCARD extern String FileContentToString(const std::filesystem::path& p);

/**
    @brief Returns the number of items in a given directory
*/
YEAGER_NODISCARD extern size_t NumberOfFilesInDir(const std::filesystem::path& p);

/**
    @brief Returns a filename without the extension at the end, for example: "model.obj" returns "model" and "audio.wav" returns "audio"
*/
YEAGER_NODISCARD extern String RemoveExtensionFromFilename(String filename);

/**
    @brief Returns the extension string from a filename, for example, "model.obj" returns ".obj" and "audio.wav" returns ".wav"
*/
YEAGER_NODISCARD extern String ExtractExtensionFromFilename(String filename);

/** 
    @brief Used in the quick serialization of files, creates a file, writes the data and closes it. Returns a boolean represeting the success of the operation 
*/
template <typename Type>
bool CreateFileAndWrites(const std::filesystem::path& path, const Type& data)
{
  std::ofstream file(path.string());

  if (!file.is_open()) {
    Yeager::LogDebug(ERROR, "Cannot opens file for writing: {}", path.string());
    return false;
  }

  file << data;
  file.close();
  return true;
}

/**
    @brief Returns a vector of string with the subdirectories of the specific path. The first element is the root of the system, and the last element
    is the last directory/ filename of the path passed. Ex: the path "C:\\Program Files\\YeagerEngine\\Configuration\\Shaders.yml returns the vector 
    { "C:", "YeagerEngine", "Configuration", "Shaders.yml" }
*/
YEAGER_NODISCARD extern std::vector<String> RetriveSubdirectoriesFromPath(const std::filesystem::path& p);

/**
    @brief Given two paths, return the common subdirectories that both paths share with eachother. Ex: Given p1 = /usr/share/bin/YeagerEngine and p2 = /usr/share/local
    it returns /usr/share, because both paths have the common location in /usr/share
*/
YEAGER_NODISCARD extern std::filesystem::path FindCommonDirectoryFromPaths(const std::filesystem::path& p1,
                                                                           const std::filesystem::path& p2);
/**
    @brief  Given two paths, return the common subdirectories that both paths share with eachother, and a two vectors of string with the directories that the two paths dont share. 
    Ex: Given p1 = /usr/share/bin/YeagerEngine and p2 = /usr/share/local it returns the shared path /usr/share, and two vectors, the first = { "bin", "YeagerEngine" } and the second
    { "local" }. The return type is a tuple of type <std::filesystem::path, std::vector<String>, std::vector<String>> 
*/
YEAGER_NODISCARD extern std::tuple<std::filesystem::path, std::vector<String>, std::vector<String>>
FindCommonDirectoryFromPathsAndDifference(const std::filesystem::path& p1, const std::filesystem::path& p2);

/** 
    @brief Retrives the complete path relative to the local folder of the engine, for Windows is the %appdata%\YeagerEngine folder and for linux is the 
    $HOME/.local/share/YeagerEngine folder. You can pass the path in the linux format of path separator, Ex: /Configuration/Variables, 
    and if the system is a windows machine, the functions converts to the windows format, \Configuration\Variables 
 */
YEAGER_NODISCARD std::optional<String> GetPathFromLocal(const String& path);

/** 
    @brief Retrives the complete path relative to the shared folder of the engine, for Windows is the (Drive Letter):\Program Files\YeagerEngine folder and for linux is the 
    /usr/share/YeagerEngine folder. You can pass the path in the linux format of path separator, Ex: /Configuration/Variables, 
    and if the system is a windows machine, the functions converts to the windows format, \Configuration\Variables 
 */
YEAGER_NODISCARD std::optional<String> GetPathFromShared(const String& path);

/**
    @brief Self explains itself, it dumps all the console strings (logs) into a file in the given path
*/
extern void DumpConsoleDataInFile(const std::filesystem::path& path);

}  // namespace Yeager