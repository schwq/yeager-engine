#include "Utilities.h"

Yeager::MemoryManagement Yeager::s_MemoryManagement;

YgString RemoveSuffixUntilCharacter(YgString expression, char characterToStop)
{
  bool finished = false;
  while (!finished) {
    expression.erase(expression.length() - 1);
    if (expression.back() == characterToStop) {
      finished = true;
    }
  }
  return expression;
}
YgString RemovePreffixUntilCharacter(YgString expression, char characterToStop)
{
  bool finished = false;
  while (!finished) {
    expression.erase(expression.begin());
    if (expression.front() == characterToStop) {
      finished = true;
    }
  }
  return expression;
}

YgString kPath = std::filesystem::current_path().string();

#ifdef YEAGER_SYSTEM_WINDOWS_x64
YgCchar kOperatingSystem = "WIN32";
#elif defined(YEAGER_SYSTEM_LINUX)
yg_cchar kOperatingSystem = "LINUX";
#endif
YgCchar GetShaderPath(YgString shader)
{
  YgString path = GetPath("Assets/shader") + YG_PS + shader;
  YgCchar rt = path.c_str();
  return rt;
}

void* operator new(size_t s)
{
  Yeager::s_MemoryManagement.m_MemoryAllocatedSize += s;
  return malloc(s);
}

void operator delete(void* ptr, size_t s)
{
  Yeager::s_MemoryManagement.m_MemoryFreedSize += s;
  free(ptr);
}
uint32_t Yeager::MemoryManagement::GetMemortUsage()
{
  return m_MemoryAllocatedSize - m_MemoryFreedSize;
}

YgString GetPath(YgString path)
{
  if (kOperatingSystem == "LINUX") {
    return kPath + "/Apps/Yeager" + path;
  } else if (kOperatingSystem == "WIN32") {
    std::replace(kPath.begin(), kPath.end(), '/', '\\');
    std::replace(path.begin(), path.end(), '/', '\\');
    return kPath + "\\apps\\Yeager" + path;
  } else {
    YgString respond = kPath + path;
    VLOG_F(ERROR,
           "GetPath function and this program does not support other OS that "
           "arent Windows and Linux system! Returing: %s",
           respond.c_str());
    return respond;
  }
};

YgString kDefaultTexturePath = GetPath("/Assets/textures/default.jpg");

unsigned int ygWindowWidth = 1300;
unsigned int ygWindowHeight = 720;