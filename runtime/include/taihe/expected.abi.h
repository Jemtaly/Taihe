#pragma once

#include <taihe/common.h>
#include <taihe/string.abi.h>

struct TError {
  int32_t code;
  struct TString message;
};
