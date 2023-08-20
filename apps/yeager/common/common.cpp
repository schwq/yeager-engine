#include "common.h"

String kPath = std::filesystem::current_path().string();

#ifdef __WIN32
const char* kOperatingSystem = "WIN32";
#elif __linux__
const char* kOperatingSystem = "LINUX";
#endif

String GetPath(String path) {
    if(kOperatingSystem == "LINUX") {
        return kPath + path;
    } else if(kOperatingSystem == "WIN32") {
        std::replace(kPath.begin(), kPath.end(), '/', '\\');
        std::replace(path.begin(), path.end(), '/', '\\');
        return kPath + path;
    }
};


unsigned int kWindowX = 800;
unsigned int kWindowY = 800;