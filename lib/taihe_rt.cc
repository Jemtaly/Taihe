#include "taihe_rt.h"

#include <cassert>

namespace taihe {

TKlass TString::Klass = {.dtor = TString::dtor};
TString::TString(std::string &&s) : TObject(&Klass), std::string(s) {}
void TString::dtor(void *self) { delete reinterpret_cast<TString *>(self); }

TKlass TList::Klass = {.dtor = TList::dtor};
void TList::dtor(void *self) { delete reinterpret_cast<TList *>(self); }
TList::TList() : TObject(&Klass) {}
}  // namespace taihe
