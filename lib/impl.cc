#include "impl.h"

#include <string>
#include <vector>

namespace api_impl {
int plus(int a, int b) { return a + b; }

std::string concat(std::vector<std::string> xs) {
  std::string ret;
  for (const auto &x : xs) {
    if (x.empty()) continue;
    ret += x;
    ret += ';';
  }

  // Remove the trailing separator ';'.
  if (!ret.empty()) ret.pop_back();

  return ret;
}

}  // namespace api_impl
