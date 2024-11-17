#include "Settings.h"
#include "Application.h"
using namespace Yeager;

AntiAliasingOption::Enum AntiAliasingOption::toEnum(String str)
{
  switch (Yeager::StringToInterger(str.c_str())) {
    case Yeager::StringToInterger("MSAA x2"):
      return AntiAliasingOption::MSAA_x2;
    case Yeager::StringToInterger("MSAA x4"):
      return AntiAliasingOption::MSAA_x4;
    default:
      Yeager::Log(WARNING, "Anti Aliasing Option to enum, invalid string! {}", str);
      return AntiAliasingOption::MSAA_x2;
  }
}

String AntiAliasingOption::toString(AntiAliasingOption::Enum type)
{
  switch (type) {
    case AntiAliasingOption::MSAA_x2:
      return "MSAA x2";
    case AntiAliasingOption::MSAA_x4:
      return "MSAA x4";
    default:
      Yeager::Log(WARNING, "Anti Aliasing Option to string, invalid enum!");
      return "MSAA x2";
  }
}

Settings::Settings(Yeager::ApplicationCore* application) : m_Application(application)
{
  InitializeDefaultValues();
}

Settings::~Settings() {}

void Settings::InitializeDefaultValues() {}

bool Settings::ChangeVideoSettingsAntiAliasingType(AntiAliasingOption::Enum type)
{
  switch (type) {
    case AntiAliasingOption::MSAA_x2: {
      if (m_Application->GetWindow()->ChangeAntiAliasingSamples(2)) {
        m_VideoSettings.AntiAliasingType = type;
        m_Application->GetRequestHandle()->Call(RequestTypeHdl::RebuildGLFWWindow);
        return true;
      }
    } break;
    case AntiAliasingOption::MSAA_x4: {

      if (m_Application->GetWindow()->ChangeAntiAliasingSamples(4)) {
        m_VideoSettings.AntiAliasingType = type;
        m_Application->GetRequestHandle()->Call(RequestTypeHdl::RebuildGLFWWindow);
        return true;
      }
    } break;
  }
  return false;
}
