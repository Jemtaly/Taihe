#include "impl.h"

#include <string>
#include <vector>

#include "taihe_rt.h"

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

namespace api_taihe_impl {
using taihe::TList;
using taihe::TString;

TString *concat(TList *xs) {
  std::string ret;

  for (const auto &o : *xs) {
    TString *x = o.cast<TString>();
    if (x->empty()) continue;
    ret += *x;
    ret += ';';
  }

  // Remove the trailing separator ';'.
  if (!ret.empty()) ret.pop_back();

  return new TString(std::move(ret));
}
}  // namespace api_taihe_impl
