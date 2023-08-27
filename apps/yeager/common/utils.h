#pragma once

#include "common.h"

extern constexpr String RemoveSuffixUntilCharacter(String expression,
                                                   char characterToStop);
extern constexpr String RemovePreffixUntilCharacter(String expression,
                                                    char characterToStop);
template <typename T>
constexpr void EraseVector(std::vector<T>* vector) {
  for (uint x = 0; x < vector.size(); x++) {
    delete vector[x];
  }
}