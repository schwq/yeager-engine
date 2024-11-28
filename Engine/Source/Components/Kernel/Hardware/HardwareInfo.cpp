#include "HardwareInfo.h"
using namespace Yeager;

#if (__cplusplus >= YEAGER_CPP_11_VERSION)
Uint Yeager::GetHardwareThreadCount()
{
  return std::thread::hardware_concurrency();
}
#else
#ifdef YEAGER_SYSTEM_WINDOWS_x64
Uint Yeager::GetHardwareThreadCount()
{
  SYSTEM_INFO sysinfo;
  GetSystemInfo(&sysinfo);
  return sysinfo.dwNumberOfProcessors;
}
#elif defined(YEAGER_SYSTEM_LINUX)
Uint Yeager::GetHardwareThreadCount()
{
  return __sysconf(_GLIBCXX_USE_SC_NPROCESSORS_ONLN);
}
#else
#error "Yeager Engine build cannot find the processors thread count on the hardware!"
#endif
#endif
