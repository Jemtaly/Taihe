#pragma once
#include <string>
#include <vector>

#include "taihe_rt.h"

namespace api_impl {
int plus(int a, int b);
std::string concat(std::vector<std::string> xs);
}  // namespace api_impl

namespace api_taihe_impl {
using taihe::TList;
using taihe::TString;

TString *concat(TList *xs);
}  // namespace api_taihe_impl
