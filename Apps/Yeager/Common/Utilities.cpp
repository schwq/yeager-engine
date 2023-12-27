#include "Utilities.h"
#include "LogEngine.h"

#ifdef YEAGER_SYSTEM_LINUX
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#endif

Yeager::MemoryManagement Yeager::s_MemoryManagement;

glm::mat4 Yeager::ConvertAssimpMatrixToGLMFormat(const aiMatrix4x4& from)
{
  glm::mat4 to;
  to[0][0] = from.a1;
  to[1][0] = from.a2;
  to[2][0] = from.a3;
  to[3][0] = from.a4;
  to[0][1] = from.b1;
  to[1][1] = from.b2;
  to[2][1] = from.b3;
  to[3][1] = from.b4;
  to[0][2] = from.c1;
  to[1][2] = from.c2;
  to[2][2] = from.c3;
  to[3][2] = from.c4;
  to[0][3] = from.d1;
  to[1][3] = from.d2;
  to[2][3] = from.d3;
  to[3][3] = from.d4;
  return to;
}

size_t NumberOfFilesInDir(YgString path)
{
  std::filesystem::path check_p = path;
  using std::filesystem::directory_iterator;
  return std::distance(directory_iterator(check_p), directory_iterator{});
}

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
YgCchar kOperatingSystem = "LINUX";
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
  Yeager::s_MemoryManagement.m_NumberNewCalls += 1;
  return malloc(s);
}

void operator delete(void* ptr, size_t s)
{
  Yeager::s_MemoryManagement.m_MemoryFreedSize += s;
  Yeager::s_MemoryManagement.m_NumberDeleteCalls += 1;
  if (ptr) {
    free(ptr);
  }
}
uint32_t Yeager::MemoryManagement::GetMemortUsage()
{
  return m_MemoryAllocatedSize - m_MemoryFreedSize;
}

YgString GetPath(YgString path)
{
  if (kOperatingSystem == "LINUX") {
    Yeager::ValidatesPath(kPath + "/Apps/Yeager" + path);
    return kPath + "/Apps/Yeager" + path;
  } else if (kOperatingSystem == "WIN32") {
    std::replace(kPath.begin(), kPath.end(), '/', '\\');
    std::replace(path.begin(), path.end(), '/', '\\');
    Yeager::ValidatesPath(kPath + "\\apps\\Yeager" + path);
    return kPath + "\\apps\\Yeager" + path;
  } else {
    YgString respond = kPath + path;
    VLOG_F(ERROR,
           "GetPath function and this program does not support other OS that "
           "arent Windows and Linux system! Returing: %s",
           respond.c_str());
    Yeager::ValidatesPath(respond);
    return respond;
  }
};

YgString kDefaultTexturePath = GetPath("/Assets/textures/default.jpg");

unsigned int ygWindowWidth = 1300;
unsigned int ygWindowHeight = 720;

extern YgString CurrentTimeFormatToString()
{
  std::time_t current_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  YgString s_time = std::ctime(&current_time);
  return s_time;
}
extern YgTime_t CurrentTimeToTimeType()
{
  YgTime_t time;

  std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
  time_t tt = std::chrono::system_clock::to_time_t(now);
  tm utc_tm = *gmtime(&tt);
  tm local_tm = *localtime(&tt);

  time.Date.Year = local_tm.tm_year;
  time.Date.Month = local_tm.tm_mon;
  time.Date.Day = local_tm.tm_mday;
  time.Date.WeekDay = local_tm.tm_wday;

  time.Time.Hours = local_tm.tm_hour;
  time.Time.Minutes = local_tm.tm_min;
  time.Time.Seconds = local_tm.tm_sec;

  return time;
}

bool Yeager::ValidatesPath(const std::filesystem::path& p, std::filesystem::file_status s)
{
  YgString status = (std::filesystem::status_known(s) ? std::filesystem::exists(s) : std::filesystem::exists(p))
                        ? "Validated!"
                        : "Not Validated!";
#ifdef YEAGER_DEBUG
  Yeager::Log((status == "Validated!" ? INFO : ERROR), "File validation: Path: {}, Status [{}]", p.c_str(), status);
#endif
  return (status == "Validated!");
}

#ifdef YEAGER_SYSTEM_LINUX 
YgString GetLinuxHomeDirectory() {
  const char* homeDir;
  /* The $HOME env varaible does not exist */
  if((homeDir = getenv("HOME")) == NULL) {
    homeDir = getpwuid(getuid())->pw_dir;
    return YgString(homeDir);
  } else {
    /* The $HOME env variable exists!*/
    return YgString(getenv("HOME"));
  }
}
#endif