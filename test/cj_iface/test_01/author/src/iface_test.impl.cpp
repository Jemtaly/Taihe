#include "iface_test.proj.hpp"
#include "iface_test.impl.hpp"
#include "stdexcept"
using namespace taihe;

class Noo {
  string name_{"noo"};
  ::taihe::optional<int32_t> age_{::taihe::optional<int32_t>(std::in_place, 1)};

public:
  void Bar() {
    std::cout << "Nooimpl: " << __func__ << std::endl;
  }

  string GetName() {
    std::cout << "Nooimpl: " << __func__ << " " << name_ << std::endl;
    return name_;
  }

  ::taihe::optional<int32_t> GetAge() {
    return age_;
  }

  void SetAge(::taihe::optional_view<int32_t> a) {
    this->age_ = a;
    return;
  }
};

::iface_test::Noo GetNooIface() {
    // The parameters in the make_holder function should be of the same type
    // as the parameters in the constructor of the actual implementation class.
    return taihe::make_holder<Noo, ::iface_test::Noo>();
}

::taihe::string PrintNooName(::iface_test::weak::Noo noo) {
    auto name = noo->GetName();
    std::cout << __func__ << ": " << name << std::endl;
    return name;
}

// Since these macros are auto-generate, lint will cause false positive.
// NOLINTBEGIN
TH_EXPORT_CPP_API_GetNooIface(GetNooIface);
TH_EXPORT_CPP_API_PrintNooName(PrintNooName);
// NOLINTEND
