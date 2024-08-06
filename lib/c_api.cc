#include "c_api.h"

#include <string.h>

#include "impl.h"

extern "C" {
int OH_Plus(int a, int b) { return api_impl::plus(a, b); }

char *OH_Concat(const char **args) {
  std::vector<std::string> xs;
  for (; *args; ++args) xs.push_back(*args);
  std::string ret = api_impl::concat(xs);
  return strdup(ret.c_str());
}
}
