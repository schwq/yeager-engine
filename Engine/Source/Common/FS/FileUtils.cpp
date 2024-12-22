#include "FileUtils.h"
using namespace Yeager;

FileHandle Yeager::OpenFileR(const String& path, std::ios_base::openmode mode)
{
  FileHandle handle = Yeager::OpenFileW(path, mode);
  handle.mSize = Yeager::GetFileSize(handle);
  return handle;
}

FileHandle Yeager::OpenFileW(const String& path, std::ios_base::openmode mode)
{
  FileHandle handle;
  handle.mFile = std::fstream(path.c_str(), mode);

  if (!handle.mFile.is_open()) {
    Yeager::LogDebug(ERROR, "Cannot open {}", path);
    return handle;
  }

  handle.mFile.seekg(0, std::ios::beg);
  handle.mPath = path;
  handle.bValid = true;
}

std::size_t Yeager::GetFileSize(FileHandle& handle)
{
  if (handle.mFile.is_open() && handle.mFile.good()) {
    handle.mFile.seekg(0, std::ios::end);
    std::size_t size = handle.mFile.tellg();
    handle.mFile.seekg(0, std::ios::beg);
    return size;
  }
  return 0;
}

bool Yeager::CloseFile(FileHandle& handle)
{
  if (!handle.mFile.good()) {
    Yeager::LogDebug(ERROR, "The current file {} is not good!", handle.mPath);
  }

  if (handle.mFile.is_open()) {
    handle.mFile.close();
    handle.bValid = false;
    handle.mPath = YEAGER_NULL_LITERAL;
    handle.mSize = 0;
    return true;
  }
  Yeager::LogDebug(ERROR, "File cannot be closed: {}", handle.mPath);
  return false;
}
