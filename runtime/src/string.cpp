#include <taihe/string.abi.h>

#include <algorithm>

uint32_t tstr_encoding(struct TString tstr) {
  return (tstr.flags & TSTRING_ENCODING_MASK);
}

char *tstr_initialize(struct TString *tstr_ptr, uint32_t capacity) {
  size_t char_size = sizeof(char);
  size_t bytes_required = sizeof(struct TStringInfo) + capacity * char_size;
  struct TStringInfo *sh =
      reinterpret_cast<struct TStringInfo *>(malloc(bytes_required));
  if (!sh) return nullptr;

  tref_init(&sh->count, 1);
  sh->external_obj = nullptr;
  sh->dest = nullptr;

  char* buffer = reinterpret_cast<char*>(sh + 1);

  tstr_ptr->flags = TSTRING_UTF8;
  tstr_ptr->pstrinfo = sh;
  tstr_ptr->ptr = buffer;

  return buffer;
}

uint16_t *tstr_initialize_utf16(struct TString *tstr_ptr, uint32_t capacity) {
  size_t char_size = sizeof(uint16_t);
  size_t bytes_required = sizeof(struct TStringInfo) + char_size * capacity;
  struct TStringInfo *sh =
      reinterpret_cast<struct TStringInfo *>(malloc(bytes_required));
  if (!sh) return nullptr;

  tref_init(&sh->count, 1);
  sh->external_obj = nullptr;
  sh->dest = nullptr;

  char* buffer = reinterpret_cast<char*>(sh + 1);

  tstr_ptr->flags = TSTRING_UTF16;
  tstr_ptr->pstrinfo = sh;
  tstr_ptr->ptr = buffer;
  return reinterpret_cast<uint16_t *>(buffer);
}

uint32_t *tstr_initialize_utf32(struct TString *tstr_ptr, uint32_t capacity) {
  size_t char_size = sizeof(uint32_t);
  size_t bytes_required = sizeof(struct TStringInfo) + char_size * capacity;
  struct TStringInfo *sh =
      reinterpret_cast<struct TStringInfo *>(malloc(bytes_required));
  if (!sh) return nullptr;

  tref_init(&sh->count, 1);
  sh->external_obj = nullptr;
  sh->dest = nullptr;

  char* buffer = reinterpret_cast<char*>(sh + 1);

  tstr_ptr->flags = TSTRING_UTF32;
  tstr_ptr->pstrinfo = sh;
  tstr_ptr->ptr = buffer;
  return reinterpret_cast<uint32_t *>(buffer);
}

struct TString tstr_new(char const *value TH_NONNULL, size_t len) {
  struct TString tstr;
  char *buf = tstr_initialize(&tstr, len + 1);
  buf = std::copy(value, value + len, buf);
  *buf = '\0';
  tstr.length = len;
  return tstr;
}

struct TString tstr_new_utf16(uint16_t const *value TH_NONNULL, size_t len) {
  struct TString tstr;
  uint16_t *buf = tstr_initialize_utf16(&tstr, len + 1);
  std::copy(value, value + len, buf);
  buf[len] = u'\0';
  tstr.length = len * sizeof(uint16_t);
  return tstr;
}

struct TString tstr_new_utf32(uint32_t const *value TH_NONNULL, size_t len) {
  struct TString tstr;
  uint32_t *buf = tstr_initialize_utf32(&tstr, len + 1);
  std::copy(value, value + len, buf);
  buf[len] = U'\0';
  tstr.length = len * sizeof(uint32_t);
  return tstr;
}

struct TString tstr_new_ref(char const *buf TH_NONNULL, size_t len) {
  struct TString tstr;
  tstr.flags = TSTRING_REF | TSTRING_UTF8;
  tstr.length = len;
  tstr.pstrinfo = nullptr;
  tstr.ptr = buf;
  return tstr;
}

struct TString tstr_new_ref_utf16(uint16_t const *buf TH_NONNULL, size_t len) {
  struct TString tstr;
  tstr.flags = TSTRING_REF | TSTRING_UTF16;
  tstr.length = len * 2;
  tstr.pstrinfo = nullptr;
  tstr.ptr = reinterpret_cast<char const *>(buf);
  return tstr;
}

struct TString tstr_new_ref_utf32(uint32_t const *buf TH_NONNULL, size_t len) {
  struct TString tstr;
  tstr.flags = TSTRING_REF | TSTRING_UTF32;
  tstr.length = len * 4;
  tstr.pstrinfo = nullptr;
  tstr.ptr = reinterpret_cast<char const *>(buf);
  return tstr;
}

struct TString tstr_new_from_external(
    char const *buf TH_NONNULL,
    size_t len,
    void* external_obj,
    void (*dest)(void*)) {
  struct TString tstr;
  struct TStringInfo* info = reinterpret_cast<struct TStringInfo*>(
      malloc(sizeof(struct TStringInfo)));
  if (!info) {
    tstr.flags = 0;
    tstr.ptr = nullptr;
    tstr.pstrinfo = nullptr;
    tstr.length = 0;
    return tstr;
  }
  tref_init(&info->count, 1);
  info->external_obj = external_obj;
  info->dest = dest;
  tstr.flags = TSTRING_UTF8 | TSTRING_EXT;
  tstr.length = len;
  tstr.pstrinfo = info;
  tstr.ptr = buf;

  return tstr;
}

struct TString tstr_new_from_external_utf16(
    uint16_t const *buf TH_NONNULL,
    size_t len,
    void* external_obj,
    void (*dest)(void*)) {
  struct TString tstr;
  struct TStringInfo* info = reinterpret_cast<struct TStringInfo*>(
      malloc(sizeof(struct TStringInfo)));
  if (!info) {
    tstr.flags = 0;
    tstr.ptr = nullptr;
    tstr.pstrinfo = nullptr;
    tstr.length = 0;
    return tstr;
  }
  tref_init(&info->count, 1);
  info->external_obj = external_obj;
  info->dest = dest;
  tstr.flags = TSTRING_UTF16 | TSTRING_EXT;
  tstr.length = len;
  tstr.pstrinfo = info;
  tstr.ptr = reinterpret_cast<char const *>(buf);

  return tstr;
}

struct TString tstr_new_from_external_utf32(
    uint32_t const *buf TH_NONNULL,
    size_t len,
    void* external_obj,
    void (*dest)(void*)) {
  struct TString tstr;
  struct TStringInfo* info = reinterpret_cast<struct TStringInfo*>(
      malloc(sizeof(struct TStringInfo)));
  if (!info) {
    tstr.flags = 0;
    tstr.ptr = nullptr;
    tstr.pstrinfo = nullptr;
    tstr.length = 0;
    return tstr;
  }
  tref_init(&info->count, 1);
  info->external_obj = external_obj;
  info->dest = dest;
  tstr.flags = TSTRING_UTF32 | TSTRING_EXT;
  tstr.length = len;
  tstr.pstrinfo = info;
  tstr.ptr = reinterpret_cast<char const *>(buf);

  return tstr;
}

struct TString tstr_dup(struct TString orig) {
  // ref 需创建堆内存
  // sh、external 不需要创建新的堆内存
  if ((orig.flags & TSTRING_REF) == 0) {
    tref_inc(&orig.pstrinfo->count);
    return orig;
  }
  struct TString tstr;
  size_t char_size =
  (orig.flags & TSTRING_UTF16) ? 2 :
  (orig.flags & TSTRING_UTF32) ? 4 : 1;
  size_t bytes_required = sizeof(struct TStringInfo) + orig.length + char_size;
  struct TStringInfo *newSh =
      reinterpret_cast<struct TStringInfo *>(malloc(bytes_required));
  tref_init(&newSh->count, 1);
  newSh->external_obj = nullptr;
  newSh->dest = nullptr;

  char* buffer = reinterpret_cast<char*>(newSh + 1);
  std::copy(orig.ptr, orig.ptr + orig.length, buffer);
  if (orig.flags & TSTRING_UTF16) {
    uint16_t* dst = reinterpret_cast<uint16_t *>(buffer);
    dst[orig.length / 2] = u'\0';
  } else if (orig.flags & TSTRING_UTF32) {
    uint32_t* dst = reinterpret_cast<uint32_t *>(buffer);
    dst[orig.length / 4] = U'\0';
  } else {
    buffer[orig.length] = '\0';
  }

  tstr.flags = orig.flags & TSTRING_ENCODING_MASK;
  tstr.length = orig.length;
  tstr.pstrinfo = newSh;
  tstr.ptr = buffer;
  return tstr;
}

void tstr_drop(struct TString tstr) {
  if (tstr.flags & TSTRING_REF) {
    return;
  }
  struct TStringInfo *sh = tstr.pstrinfo;
  if (!sh) {
    return;
  }
  if (tref_dec(&sh->count)) {
    if ((tstr.flags & TSTRING_EXT) && sh->dest) {
      sh->dest(sh->external_obj);
    }
    free(sh);
  }
}

struct TString tstr_concat_utf8(size_t count, struct TString const *tstr_list) {
  size_t len = 0;
  for (size_t i = 0; i < count; ++i) {
    len += tstr_list[i].length;
  }
  struct TString tstr;
  char *buf = tstr_initialize(&tstr, len + 1);
  for (size_t i = 0; i < count; ++i) {
    buf = std::copy(tstr_list[i].ptr, tstr_list[i].ptr + tstr_list[i].length,
                    buf);
  }
  *buf = '\0';
  tstr.length = len;
  return tstr;
}

struct TString tstr_concat_utf16(size_t count,
                                 struct TString const *tstr_list) {
  size_t len = 0;
  for (size_t i = 0; i < count; ++i) {
    len += tstr_list[i].length;
  }
  struct TString tstr;
  uint16_t *buf = tstr_initialize_utf16(&tstr, len / 2 + 1);
  for (size_t i = 0; i < count; ++i) {
    buf = std::copy(reinterpret_cast<uint16_t const *>(tstr_list[i].ptr),
                    reinterpret_cast<uint16_t const *>(tstr_list[i].ptr) +
                        tstr_list[i].length / 2,
                    buf);
  }
  buf[len / 2] = u'\0';
  tstr.length = len;
  return tstr;
}

struct TString tstr_concat_utf32(size_t count,
                                 struct TString const *tstr_list) {
  size_t len = 0;
  for (size_t i = 0; i < count; ++i) {
    len += tstr_list[i].length;
  }
  struct TString tstr;
  uint32_t *buf = tstr_initialize_utf32(&tstr, len / 4 + 1);
  for (size_t i = 0; i < count; ++i) {
    buf = std::copy(reinterpret_cast<uint32_t const *>(tstr_list[i].ptr),
                    reinterpret_cast<uint32_t const *>(tstr_list[i].ptr) +
                        tstr_list[i].length / 4,
                    buf);
  }
  buf[len / 4] = U'\0';
  tstr.length = len;
  return tstr;
}

struct TString tstr_concat(size_t count, struct TString const *tstr_list) {
  int32_t flags = 0;
  for (size_t i = 0; i < count; ++i) {
    flags |= tstr_list[i].flags;
  }
  switch (flags & TSTRING_ENCODING_MASK) {
  case TSTRING_UTF8:
    return tstr_concat_utf8(count, tstr_list);
  case TSTRING_UTF16:
    return tstr_concat_utf16(count, tstr_list);
  case TSTRING_UTF32:
    return tstr_concat_utf32(count, tstr_list);
  }
  return tstr_list[0];
}

struct TString tstr_substr_utf8(struct TString tstr, size_t pos, size_t len) {
  if (pos > tstr.length) {
    len = 0;
  } else if (pos + len > tstr.length) {
    len = tstr.length - pos;
  }
  return tstr_new_ref(tstr.ptr + pos, len);
}

struct TString tstr_substr_utf16(struct TString tstr, size_t pos, size_t len) {
  if (pos > tstr.length / 2) {
    len = 0;
  } else if (pos + len > tstr.length / 2) {
    len = tstr.length / 2 - pos;
  }
  return tstr_new_ref_utf16(reinterpret_cast<uint16_t const *>(tstr.ptr) + pos,
                            len);
}

struct TString tstr_substr_utf32(struct TString tstr, size_t pos, size_t len) {
  if (pos > tstr.length / 4) {
    len = 0;
  } else if (pos + len > tstr.length / 4) {
    len = tstr.length / 4 - pos;
  }
  return tstr_new_ref_utf32(reinterpret_cast<uint32_t const *>(tstr.ptr) + pos,
                            len);
}

struct TString tstr_substr(struct TString tstr, size_t pos, size_t len) {
  switch (tstr.flags & TSTRING_ENCODING_MASK) {
  case TSTRING_UTF8:
    return tstr_substr_utf8(tstr, pos, len);
  case TSTRING_UTF16:
    return tstr_substr_utf16(tstr, pos, len);
  case TSTRING_UTF32:
    return tstr_substr_utf32(tstr, pos, len);
  }
  return tstr;
}
