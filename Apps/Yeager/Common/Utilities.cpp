#include "Utilities.h"

yg_string RemoveSuffixUntilCharacter(yg_string expression, char characterToStop)
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
yg_string RemovePreffixUntilCharacter(yg_string expression, char characterToStop)
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

yg_string kPath = std::filesystem::current_path().string();

#ifdef YEAGER_SYSTEM_WINDOWS_x64
yg_cchar kOperatingSystem = "WIN32";
#elif defined(YEAGER_SYSTEM_LINUX)
yg_cchar kOperatingSystem = "LINUX";
#endif
yg_cchar GetShaderPath(yg_string shader)
{
  yg_string path = GetPath("Assets/shader") + YG_PS + shader;
  yg_cchar rt = path.c_str();
  return rt;
}

yg_string GetPath(yg_string path)
{
  if (kOperatingSystem == "LINUX") {
    return kPath + "/Apps/Yeager" + path;
  } else if (kOperatingSystem == "WIN32") {
    std::replace(kPath.begin(), kPath.end(), '/', '\\');
    std::replace(path.begin(), path.end(), '/', '\\');
    return kPath + "\\apps\\Yeager" + path;
  } else {
    yg_string respond = kPath + path;
    VLOG_F(ERROR,
           "GetPath function and this program does not support other OS that "
           "arent Windows and Linux system! Returing: %s",
           respond.c_str());
    return respond;
  }
};

yg_string kDefaultTexturePath = GetPath("/Assets/textures/default.jpg");

unsigned int ygWindowWidth = 1300;
unsigned int ygWindowHeight = 720;