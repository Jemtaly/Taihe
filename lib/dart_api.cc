#include </opt/dart-sdk/include/dart_api.h>

#include <cstdint>
#include <string>

#include "impl.h"
#include "taihe_rt.h"

uint8_t *utf8(std::string &s) { return reinterpret_cast<uint8_t *>(s.data()); }

extern "C" Dart_Handle dart_concat(Dart_Handle args) {
  Dart_EnterScope();

  assert(Dart_IsList(args));
  intptr_t n_strs;
  Dart_ListLength(args, &n_strs);

  // Convert Dart List to C++ vector of Dart strings
  taihe::TList *ss = new taihe::TList();
  ss->reserve(n_strs);
  for (intptr_t i = 0; i < n_strs; i++) {
    Dart_Handle dstr = Dart_ListGetAt(args, i);
    assert(Dart_IsString(dstr));

    intptr_t slen;
    std::string stdstr;
    Dart_StringUTF8Length(dstr, &slen);
    stdstr.resize(slen);
    Dart_CopyUTF8EncodingOfString(dstr, utf8(stdstr), slen);
    auto tstr = taihe::make_sptr<taihe::TString>(std::move(stdstr));
    ss->emplace_back(tstr.cast<taihe::TObject>());
  }

  taihe::TString *tret = api_taihe_impl::concat(ss);
  Dart_Handle dartResult = Dart_NewStringFromUTF8(utf8(*tret), tret->size());

  ss->unref();
  tret->unref();
  return dartResult;
}
