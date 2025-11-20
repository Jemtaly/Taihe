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

inline size_t utf8_to_utf16_required(const char* src, size_t len) {
  const uint8_t* data = reinterpret_cast<const uint8_t*>(src);
  size_t pos = 0, units = 0;

  while (pos < len) {
    if (pos + 16 <= len) {
      uint64_t v1, v2;
      std::copy(data + pos,     data + pos + 8,  reinterpret_cast<uint8_t*>(&v1));
      std::copy(data + pos + 8, data + pos + 16, reinterpret_cast<uint8_t*>(&v2));
      uint64_t v = v1 | v2;
      if ((v & 0x8080808080808080) == 0) {
        units += 16;
        pos += 16;
        continue;
      }
    }

    uint8_t leading_byte = data[pos];
    if (leading_byte < 0b10000000) {
      // ASCII
      units += 1;
      pos++;
    } else if ((leading_byte & 0b11100000) == 0b11000000) {
      // 2 字节 UTF-8
      if (pos + 1 >= len || (data[pos + 1] & 0b11000000) != 0b10000000) {
        units += 1;
        pos++;
        continue;
      }
      units += 1;
      pos += 2;
    } else if ((leading_byte & 0b11110000) == 0b11100000) {
      // 3 字节 UTF-8
      if (pos + 2 >= len ||
          (data[pos + 1] & 0b11000000) != 0b10000000 ||
          (data[pos + 2] & 0b11000000) != 0b10000000
        ) {
        units += 1;
        pos++;
        continue;
      }
      units += 1;
      pos += 3;
    } else if ((leading_byte & 0b11111000) == 0b11110000) {
      // 4 字节 UTF-8
      if (pos + 3 >= len ||
          (data[pos + 1] & 0b11000000) != 0b10000000 ||
          (data[pos + 2] & 0b11000000) != 0b10000000 ||
          (data[pos + 3] & 0b11000000) != 0b10000000
        ) {
        units += 1;
        pos++;
        continue;
      }
      units += 2;
      pos += 4;
    } else {
      units += 1;
      pos++;
    }
  }
  return units;
}

inline size_t utf8_to_utf16(const char *input, size_t len, uint16_t *output) {
  const uint8_t *data = reinterpret_cast<const uint8_t *>(input);
  size_t pos = 0;
  uint16_t *start{output};

  while (pos < len) {
    if (pos + 16 <= len) {
      uint64_t v1, v2;
      std::copy(
        data + pos,
        data + pos + 8,
        reinterpret_cast<uint8_t*>(&v1)
      );
      std::copy(data + pos + 8, data + pos + 16, reinterpret_cast<uint8_t*>(&v2));
      uint64_t v{v1 | v2};
      if ((v & 0x8080808080808080) == 0) {
        size_t final_pos = pos + 16;
        while (pos < final_pos) {
          *output++ = uint16_t(input[pos]);
          pos++;
        }
        continue;
      }
    }

    uint8_t leading_byte = data[pos];
    if (leading_byte < 0b10000000) {
      // ASCII
      *output++ = uint16_t(leading_byte);
      pos++;
    } else if ((leading_byte & 0b11100000) == 0b11000000) {
      // 2 字节 UTF-8
      if (pos + 1 >= len || (data[pos + 1] & 0b11000000) != 0b10000000) {
        *output++ = 0xFFFD;
        pos++;
        continue;
      }
      uint32_t code_point =
          (leading_byte & 0b00011111) << 6 | (data[pos + 1] & 0b00111111);
      if (code_point < 0x80 || 0x7ff < code_point) {
        return 0;
      }
      *output++ = uint16_t(code_point);
      pos += 2;
    } else if ((leading_byte & 0b11110000) == 0b11100000) {
      // 3 字节 UTF-8
      if (pos + 2 >= len ||
          (data[pos + 1] & 0b11000000) != 0b10000000 ||
          (data[pos + 2] & 0b11000000) != 0b10000000
        ) {
        *output++ = 0xFFFD;
        pos++;
        continue;
      }
      uint32_t code_point = (leading_byte & 0b00001111) << 12 |
                            (data[pos + 1] & 0b00111111) << 6 |
                            (data[pos + 2] & 0b00111111);
      if (code_point < 0x800 || 0xffff < code_point ||
          (0xd7ff < code_point && code_point < 0xe000)) {
        return 0;
      }
      *output++ = uint16_t(code_point);
      pos += 3;
    } else if ((leading_byte & 0b11111000) == 0b11110000) {
      // 4 字节 UTF-8
      if (pos + 3 >= len ||
          (data[pos + 1] & 0b11000000) != 0b10000000 ||
          (data[pos + 2] & 0b11000000) != 0b10000000 ||
          (data[pos + 3] & 0b11000000) != 0b10000000
        ) {
        *output++ = 0xFFFD;
        pos++;
        continue;
      }
      uint32_t code_point = (leading_byte & 0b00000111) << 18 |
                            (data[pos + 1] & 0b00111111) << 12 |
                            (data[pos + 2] & 0b00111111) << 6 |
                            (data[pos + 3] & 0b00111111);
      if (code_point <= 0xffff || 0x10ffff < code_point) {
        return 0;
      }
      code_point -= 0x10000;
      uint16_t high_surrogate = uint16_t(0xD800 + (code_point >> 10));
      uint16_t low_surrogate = uint16_t(0xDC00 + (code_point & 0x3FF));
      *output++ = high_surrogate;
      *output++ = low_surrogate;
      pos += 4;
    } else {
      return 0;
    }
  }
  return output - start;
}

inline size_t utf16_to_utf8_required(const uint16_t* src, size_t len) {
  const uint16_t *data = src;
  size_t pos = 0, units = 0;
  while (pos < len) {
    if (pos + 4 <= len) {
      uint64_t v;
      std::copy(
        data + pos,
        data + pos + 4,
        reinterpret_cast<uint8_t*>(&v)
      );
      if ((v & 0xFF80FF80FF80FF80) == 0) {
        units += 4;
        pos += 4;
        continue;
      }
    }
    uint16_t word = data[pos];
    if ((word & 0xFF80) == 0) {
      units += 1;
      pos++;
    } else if ((word & 0xF800) == 0) {
      units += 2;
      pos++;
    } else if ((word & 0xF800) != 0xD800) {
      units += 3;
      pos++;
    } else {
      // must be a surrogate pair
      uint16_t diff = uint16_t(word - 0xD800);
      if (pos + 1 >= len || diff > 0x3FF) {
        units += 3;
        pos++;
        continue;
      }
      if (uint16_t(data[pos + 1] - 0xDC00) > 0x3FF) {
        units += 3;
        pos++;
        continue;
      }
      units += 4;
      pos += 2;
    }
  }
  return units;
}

inline size_t utf16_to_utf8(const uint16_t *input, size_t len, char *output) {
  const uint16_t *data = input;
  size_t pos = 0;
  char *start{output};
  while (pos < len) {
    if (pos + 4 <= len) {
      uint64_t v;
      std::copy(
        data + pos,
        data + pos + 4,
        reinterpret_cast<uint8_t*>(&v)
      );
      if ((v & 0xFF80FF80FF80FF80) == 0) {
        size_t final_pos = pos + 4;
        while (pos < final_pos) {
          *output++ = char(input[pos]);
          pos++;
        }
        continue;
      }
    }
    uint16_t word = data[pos];
    if ((word & 0xFF80) == 0) {
      *output++ = char(word);
      pos++;
    } else if ((word & 0xF800) == 0) {
      *output++ = char((word >> 6) | 0b11000000);
      *output++ = char((word & 0b111111) | 0b10000000);
      pos++;
    } else if ((word & 0xF800) != 0xD800) {
      *output++ = char((word >> 12) | 0b11100000);
      *output++ = char(((word >> 6) & 0b111111) | 0b10000000);
      *output++ = char((word & 0b111111) | 0b10000000);
      pos++;
    } else {
      // must be a surrogate pair
      uint16_t diff = uint16_t(word - 0xD800);
      if (pos + 1 >= len || diff > 0x3FF) {
        *output++ = char(0xEF);
        *output++ = char(0xBF);
        *output++ = char(0xBD);
        pos++;
        continue;
      }
      uint16_t next_word = data[pos + 1];
      uint16_t diff2 = uint16_t(next_word - 0xDC00);
      if (diff2 > 0x3FF) {
        *output++ = char(0xEF);
        *output++ = char(0xBF);
        *output++ = char(0xBD);
        pos++;
        continue;
      }

      uint32_t value = (diff << 10) + diff2 + 0x10000;
      *output++ = char((value >> 18) | 0b11110000);
      *output++ = char(((value >> 12) & 0b111111) | 0b10000000);
      *output++ = char(((value >> 6) & 0b111111) | 0b10000000);
      *output++ = char((value & 0b111111) | 0b10000000);
      pos += 2;
    }
  }
  return output - start;
}

struct TString tstr_utf8_to_utf16(struct TString utf8_str) {
  if (tstr_encoding(utf8_str) == TSTRING_UTF16) return tstr_dup(utf8_str);

  const char* src = tstr_buf(utf8_str);
  size_t len = utf8_str.length;

  size_t needed = utf8_to_utf16_required(src, len);

  struct TString result;
  uint16_t* dst = tstr_initialize_utf16(&result, (needed + 1));

  if (!dst) return (struct TString){
    .flags = TSTRING_UTF16,
    .length = 0,
    .pstrinfo = nullptr,
    .ptr = nullptr,
  };

  size_t used_len = utf8_to_utf16(src, len, dst);
  dst[used_len] = u'\0';
  result.length = used_len * 2;
  return result;
}

struct TString tstr_utf16_to_utf8(struct TString utf16_str) {
  if (tstr_encoding(utf16_str) == TSTRING_UTF8) return tstr_dup(utf16_str);

  const uint16_t* src = reinterpret_cast<const uint16_t*>(tstr_buf(utf16_str));
  size_t len = utf16_str.length / 2;

  size_t needed = utf16_to_utf8_required(src, len);

  struct TString result;
  char* dst = tstr_initialize(&result, (needed + 1));

  if (!dst) return (struct TString){
    .flags = TSTRING_UTF8,
    .length = 0,
    .pstrinfo = nullptr,
    .ptr = nullptr,
  };

  size_t used_len = utf16_to_utf8(src, len, dst);
  dst[used_len] = '\0';
  result.length = used_len;
  return result;
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
