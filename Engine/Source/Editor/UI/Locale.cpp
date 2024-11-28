#include "Locale.h"
#include "Main/Core/Application.h"
using namespace Yeager;

String ELanguangeRegion::ToString(ELanguangeRegion::Enum type)
{
  switch (type) {
    case EN_US:
      return "en-us";
    case PT_BR:
      return "pt-br";
    default:
      return YEAGER_NULL_LITERAL;
  }
}

ELanguangeRegion::Enum ELanguangeRegion::ToEnum(const String& str)
{
  switch (StringToInterger(str.c_str())) {
    case StringToInterger("en-us"):
      return EN_US;
    case StringToInterger("pt-br"):
      return PT_BR;
    default:
      return Unknown;
  }
}

std::optional<std::filesystem::path> Yeager::GetLocalePathFromLanguangeRegion(const ELanguangeRegion::Enum& language)
{
  String lang = ELanguangeRegion::ToString(language);
  if (lang == YEAGER_NULL_LITERAL)
    return std::nullopt;
  const String path_str = "/Resources/Locales/Locale_" + lang + "/Yeager_" + lang + ".yml";
  const auto path = GetPathFromShared(path_str);

  if (path.has_value())
    return path.value();
  return std::nullopt;
}

Locale::Locale(Yeager::ApplicationCore* application, const ELanguangeRegion::Enum& language, bool s_assert)
    : mLanguageRegion(language), mApplication(application)
{
  const auto lang = GetLocalePathFromLanguangeRegion(language);
  if (lang.has_value()) {
    mLocalePath = lang.value();
    bLoaded = Load(mLocalePath);
  } else {
    if (s_assert)
      static_assert("Cannot load locale path!");
  }
}

bool Locale::Load(const std::filesystem::path& path)
{
  const auto data = mApplication->GetSerial()->DeserializeLocaleData(path);
  if (data.has_value()) {
    mMapData = data.value();
    return true;
  }
  return false;
}

String Locale::Translate(String key) const
{
  if (!bLoaded)
    return YEAGER_NO_LOCALE_LOADED_STRING;
  if (mMapData.find(key) != mMapData.end())
    return mMapData.at(key);
  return YEAGER_TRANSLATE_ERROR_STRING;
}