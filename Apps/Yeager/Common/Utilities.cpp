#include "Utilities.h"
#include "LogEngine.h"

#ifdef YEAGER_SYSTEM_LINUX
#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>
#endif
using namespace Yeager;

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

#ifdef YEAGER_SYSTEM_WINDOWS_x64
Cchar Yeager::g_OperatingSystemString = YEAGER_WINDOWS32_OS_STRING;
#elif defined(YEAGER_SYSTEM_LINUX)
Cchar Yeager::g_OperatingSystemString = YEAGER_LINUX_OS_STRING;
#endif

bool Yeager::g_FromSourceCode = false;

uint64_t Yeager::Convert32BitsTo64Bits(uint32_t high, uint32_t low)
{
  return (uint64_t)high << 32 | low;
}

bool Yeager::EvaluateIntToBool(const int i)
{
  return i == 0 ? false : true;
}

std::vector<String> Yeager::RetrievePhrasesBetweenChar(String str, const char ch)
{
  std::vector<String> phrases;
  while (str.find(ch) != String::npos) {
    String ph = ReadSuffixUntilCharacter(str, ch);
    phrases.push_back(ph.substr(1, ph.size()));
    str = str.substr(0, str.size() - ph.size());
  }
  phrases.push_back(str);
  std::reverse(phrases.begin(), phrases.end());
  return phrases;
}

String Yeager::RemoveSuffixUntilCharacter(String expression, char characterToStop)
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

String Yeager::RemovePreffixUntilCharacter(String expression, char characterToStop)
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

String Yeager::ReadSuffixUntilCharacter(String expression, char characterToStop)
{
  if (expression.empty() || expression.find(characterToStop) == std::string::npos) {
    Yeager::Log(ERROR,
                "The string is empty or the character cannot be found in the expression! Expression: {}, Character {}",
                expression, characterToStop);
    return String();
  }
  size_t pos = expression.find_last_of(characterToStop);
  expression = expression.substr(pos, expression.length());

  return expression;
}

int Yeager::DisplayWarningPanicMessageBox(const String& cause, const std::filesystem::path& log)
{
#ifdef YEAGER_SYSTEM_WINDOWS_x64
  return DisplayWarningPanicMessageBoxWindows(cause, log);
#endif
  return 0;
}

#ifdef YEAGER_SYSTEM_WINDOWS_x64
int Yeager::DisplayWarningPanicMessageBoxWindows(const String& cause, const std::filesystem::path& log)
{
  if (g_OperatingSystemString == YEAGER_WINDOWS32_OS_STRING) {
    return MessageBoxW(NULL,
                       (LPCWSTR)String("A problem have occurs that causes the Yeager Engine to panic! Cause: " + cause +
                                       "\nPlease check the log generated in the path: \n" + log.string())
                           .c_str(),
                       (LPCWSTR)String("Yeager Engine panic!").c_str(), MB_ICONHAND | MB_APPLMODAL | MB_OK);
  }
  return -1;
}
#endif

#ifdef YEAGER_SYSTEM_LINUX

int Yeager::ExLinuxCmd(const String& cmd)
{
  if (!CheckIFLinuxCmdExists(cmd))
    Yeager::Log(WARNING, "Executing a unknown command on the linux machine!");
  return system(cmd.c_str());
}

bool Yeager::CheckIFLinuxCmdExists(const String& cmd)
{
  return !system(String("command -v " + cmd + " > /dev/null 2>&1").c_str());
}

#endif

String Yeager::ToLower(const String& str)
{
  String result = str;
  std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) { return std::tolower(c); });
  return result;
}

String Yeager::ToUpper(const String& str)
{
  String result = str;
  std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) { return std::toupper(c); });
  return result;
}
