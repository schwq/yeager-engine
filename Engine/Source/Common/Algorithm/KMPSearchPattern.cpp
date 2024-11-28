#include "KMPSearchPattern.h"
using namespace Yeager;

std::vector<int> Yeager::KMPStringSearch(const String& pattern, const String& entry)
{
  int M = pattern.length();
  int N = entry.length();
  std::vector<int> lsp(M);
  std::vector<int> result;
  KMPComputeLSPArray(pattern, M, lsp);

  return result;
}

void Yeager::KMPComputeLSPArray(const String& pattern, int M, std::vector<int>& lsp) {}