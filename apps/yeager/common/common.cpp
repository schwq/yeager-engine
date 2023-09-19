#include "common.h"

String kPath = std::filesystem::current_path().string();

#ifdef _WIN32
const char* kOperatingSystem = "WIN32";
#elif __linux__
const char* kOperatingSystem = "LINUX";
#endif

String GetPath(String path) {
  if (kOperatingSystem == "LINUX") {
    return kPath + path;
  } else if (kOperatingSystem == "WIN32") {
    std::replace(kPath.begin(), kPath.end(), '/', '\\');
    std::replace(path.begin(), path.end(), '/', '\\');
    return kPath + "\\apps\\yeager" + path;
  } else {
    String respond = kPath + path;
    VLOG_F(ERROR,
           "GetPath function and this program does not support other OS that "
           "arent Windows and Linux system! Returing: %s",
           respond.c_str());
    return respond;
  }
};

String kDefaultTexturePath = GetPath("/assets/textures/default.jpg");

unsigned int kWindowX = 1920;
unsigned int kWindowY = 1080;