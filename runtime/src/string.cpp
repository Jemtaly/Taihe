#include <taihe/string.abi.h>

#include <algorithm>

// Converts a TString into its corresponding heap-allocated TStringData
// structure.
//
// # Arguments
// - `tstr`: The TString to be converted.
//
// # Returns
// - A pointer to the TStringData structure if the TString is heap-allocated.
// - `nullptr` if the TString is a reference (TSTRING_REF is set).
TH_INLINE struct TStringData *to_heap(struct TString tstr) {
  if (tstr.flags & TSTRING_REF) {
    return nullptr;
  }
  return reinterpret_cast<struct TStringData *>(
      const_cast<char *>(tstr.ptr) - offsetof(struct TStringData, buffer));
}

char *tstr_initialize(struct TString *tstr_ptr, uint32_t capacity) {
  size_t char_size = sizeof(char);
  size_t bytes_required = sizeof(struct TStringData) + char_size * capacity;
  struct TStringData *sh =
      reinterpret_cast<struct TStringData *>(malloc(bytes_required));
  tref_init(&sh->count, 1);
  tstr_ptr->flags = TSTRING_UTF8;
  tstr_ptr->ptr = sh->buffer;
  return sh->buffer;
}

uint16_t *tstr_initialize_utf16(struct TString *tstr_ptr, uint32_t capacity) {
  size_t char_size = sizeof(uint16_t);
  size_t bytes_required = sizeof(struct TStringData) + char_size * capacity;
  struct TStringData *sh =
      reinterpret_cast<struct TStringData *>(malloc(bytes_required));
  tref_init(&sh->count, 1);
  tstr_ptr->flags = TSTRING_UTF16;
  tstr_ptr->ptr = sh->buffer;
  return reinterpret_cast<uint16_t *>(sh->buffer);
}

uint32_t *tstr_initialize_utf32(struct TString *tstr_ptr, uint32_t capacity) {
  size_t char_size = sizeof(uint32_t);
  size_t bytes_required = sizeof(struct TStringData) + char_size * capacity;
  struct TStringData *sh =
      reinterpret_cast<struct TStringData *>(malloc(bytes_required));
  tref_init(&sh->count, 1);
  tstr_ptr->flags = TSTRING_UTF32;
  tstr_ptr->ptr = sh->buffer;
  return reinterpret_cast<uint32_t *>(sh->buffer);
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
  tstr.ptr = buf;
  return tstr;
}

struct TString tstr_new_ref_utf16(uint16_t const *buf TH_NONNULL, size_t len) {
  struct TString tstr;
  tstr.flags = TSTRING_REF | TSTRING_UTF16;
  tstr.length = len * 2;
  tstr.ptr = reinterpret_cast<char const *>(buf);
  return tstr;
}

struct TString tstr_new_ref_utf32(uint32_t const *buf TH_NONNULL, size_t len) {
  struct TString tstr;
  tstr.flags = TSTRING_REF | TSTRING_UTF32;
  tstr.length = len * 4;
  tstr.ptr = reinterpret_cast<char const *>(buf);
  return tstr;
}

struct TString tstr_dup(struct TString orig) {
  struct TStringData *sh = to_heap(orig);
  if (sh) {
    tref_inc(&sh->count);
    return orig;
  }
  struct TString tstr;
  size_t bytes_required = sizeof(struct TStringData) + orig.length;
  struct TStringData *newSh =
      reinterpret_cast<struct TStringData *>(malloc(bytes_required));
  tref_init(&newSh->count, 1);
  std::copy(orig.ptr, orig.ptr + orig.length, newSh->buffer);
  tstr.flags = orig.flags;
  tstr.length = orig.length;
  tstr.ptr = newSh->buffer;
  return tstr;
}

void tstr_drop(struct TString tstr) {
  struct TStringData *sh = to_heap(tstr);
  if (!sh) {
    return;
  }
  if (tref_dec(&sh->count)) {
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
