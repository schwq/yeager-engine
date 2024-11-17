#include "BruteForceSearch.h"
using namespace Yeager;

std::vector<int> Yeager::BruteForceStringSearch(const String& pattern, const String& str)
{
  std::vector<int> result;
  if (pattern.length() > str.length())
    return result;  // The pattern lenght is bigger that the given string

  if (pattern == str) {
    result.push_back(0);
    return result;
  }

  for (size_t it = 0; it < str.length() - pattern.length(); it++) {
    bool matching = true;
    for (size_t p = 0; p < pattern.length(); p++) {
      if (str[it + p] != pattern[p]) {
        matching = false;  // There was a mismatch, break from the loop and increments a character in the string
        break;
      }
    }
    if (matching)
      result.push_back(it);  // The index is pushed back
  }
  return result;
}
