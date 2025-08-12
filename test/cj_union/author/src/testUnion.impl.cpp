#include "testUnion.proj.hpp"
#include "testUnion.impl.hpp"
#include "stdexcept"

using namespace taihe;

string PrintMyUnion(::testUnion::MyUnion const &data) {
  switch (data.get_tag()) {
  case ::testUnion::MyUnion::tag_t::boolValue:
    std::cout << "b: " << data.get_boolValue_ref() << std::endl;
    return "b";
  case ::testUnion::MyUnion::tag_t::floatValue:
    std::cout << "f: " << data.get_floatValue_ref() << std::endl;
    return "f";
  }
}

::testUnion::MyUnion MakeMyUnion(string_view kind) {
  float const testFloat = 123.0f;
  bool const testBool = true;
  if (kind == "b") {
    return ::testUnion::MyUnion::make_boolValue(testBool);
  }
  if (kind == "f") {
    return ::testUnion::MyUnion::make_floatValue(testFloat);
  }
}


// Since these macros are auto-generate, lint will cause false positive.
// NOLINTBEGIN
TH_EXPORT_CPP_API_PrintMyUnion(PrintMyUnion);
TH_EXPORT_CPP_API_MakeMyUnion(MakeMyUnion);
// NOLINTEND
