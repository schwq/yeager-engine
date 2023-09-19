#include "utils.h"

EditorConsole kConsole;

constexpr String RemoveSuffixUntilCharacter(String expression,
                                            char characterToStop) {
  bool finished = false;
  while (!finished) {
    expression.erase(expression.length() - 1);
    if (expression.back() == characterToStop) {
      finished = true;
    }
  }
  return expression;
}
constexpr String RemovePreffixUntilCharacter(String expression,
                                             char characterToStop) {
  bool finished = false;
  while (!finished) {
    expression.erase(expression.begin());
    if (expression.front() == characterToStop) {
      finished = true;
    }
  }
  return expression;
}