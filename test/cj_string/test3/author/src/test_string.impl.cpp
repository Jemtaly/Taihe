#include "test_string.proj.hpp"
#include "test_string.impl.hpp"

#include "stdexcept"
#include <iostream>

namespace {
// To be implemented.

::taihe::string getNewString(::test_string::in_string const& origin) {
   taihe::string res="";
   for(int i=0;i<origin.n;i++){
      res+=origin.s;
   }
   return res ;
}  // namespace
}
// Since these macros are auto-generate, lint will cause false positive.
// NOLINTBEGIN
TH_EXPORT_CPP_API_getNewString(getNewString);
// NOLINTEND
