#include <taihe/string.abi.h>

#include <algorithm>

uint32_t tstr_encoding(struct TString tstr) {
  return (tstr.flags & TSTRING_ENCODING_MASK);
}

TH_INLINE struct TStringCache *to_string_cache(struct TString tstr) {
  if (!(tstr.flags & TSTRING_HAS_CACHE)) {
    return nullptr;
  }
  return reinterpret_cast<struct TStringCache *>(
      const_cast<char *>(tstr.cache) - offsetof(struct TStringCache, buffer));
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

  char *buffer = reinterpret_cast<char *>(sh + 1);

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

  char *buffer = reinterpret_cast<char *>(sh + 1);

  tstr_ptr->flags = TSTRING_UTF16;
  tstr_ptr->pstrinfo = sh;
  tstr_ptr->ptr = buffer;
  return reinterpret_cast<uint16_t *>(buffer);
}

struct TString tstr_new(char const *value TH_NONNULL, size_t len) {
  struct TString tstr;
  char *buf = tstr_initialize(&tstr, len + 1);
  buf = std::copy(value, value + len, buf);
  *buf = '\0';
  tstr.length = len;
  tstr.cache = nullptr;
  return tstr;
}

struct TString tstr_new_utf16(uint16_t const *value TH_NONNULL, size_t len) {
  struct TString tstr;
  uint16_t *buf = tstr_initialize_utf16(&tstr, len + 1);
  std::copy(value, value + len, buf);
  buf[len] = u'\0';
  tstr.length = len * sizeof(uint16_t);
  tstr.cache = nullptr;
  return tstr;
}

struct TString tstr_new_ref(char const *buf TH_NONNULL, size_t len) {
  struct TString tstr;
  tstr.flags = TSTRING_REF | TSTRING_UTF8;
  tstr.length = len;
  tstr.pstrinfo = nullptr;
  tstr.ptr = buf;
  tstr.cache = nullptr;
  return tstr;
}

struct TString tstr_new_ref_utf16(uint16_t const *buf TH_NONNULL, size_t len) {
  struct TString tstr;
  tstr.flags = TSTRING_REF | TSTRING_UTF16;
  tstr.length = len * 2;
  tstr.pstrinfo = nullptr;
  tstr.ptr = reinterpret_cast<char const *>(buf);
  tstr.cache = nullptr;
  return tstr;
}

struct TString tstr_new_from_external(char const *buf TH_NONNULL, size_t len,
                                      void *external_obj,
                                      void (*dest)(void *)) {
  struct TString tstr;
  struct TStringInfo *info = reinterpret_cast<struct TStringInfo *>(
      malloc(sizeof(struct TStringInfo)));
  if (!info) {
    tstr.flags = 0;
    tstr.length = 0;
    tstr.pstrinfo = nullptr;
    tstr.ptr = nullptr;
    tstr.cache = nullptr;
    return tstr;
  }
  tref_init(&info->count, 1);
  info->external_obj = external_obj;
  info->dest = dest;
  tstr.flags = TSTRING_UTF8 | TSTRING_EXT;
  tstr.length = len;
  tstr.pstrinfo = info;
  tstr.ptr = buf;
  tstr.cache = nullptr;

  return tstr;
}

struct TString tstr_new_from_external_utf16(uint16_t const *buf TH_NONNULL,
                                            size_t len, void *external_obj,
                                            void (*dest)(void *)) {
  struct TString tstr;
  struct TStringInfo *info = reinterpret_cast<struct TStringInfo *>(
      malloc(sizeof(struct TStringInfo)));
  if (!info) {
    tstr.flags = 0;
    tstr.length = 0;
    tstr.pstrinfo = nullptr;
    tstr.ptr = nullptr;
    return tstr;
  }
  tref_init(&info->count, 1);
  info->external_obj = external_obj;
  info->dest = dest;
  tstr.flags = TSTRING_UTF16 | TSTRING_EXT;
  tstr.length = len;
  tstr.pstrinfo = info;
  tstr.ptr = reinterpret_cast<char const *>(buf);
  tstr.cache = nullptr;

  return tstr;
}

struct TString tstr_dup(struct TString orig) {
  // ref 需创建堆内存
  // sh、external 不需要创建新的堆内存
  if ((orig.flags & TSTRING_REF) == 0) {
    TStringCache *cache = to_string_cache(orig);
    if (cache) {
      tref_inc(&cache->count);
    }
    tref_inc(&orig.pstrinfo->count);
    return orig;
  }
  struct TString tstr;
  size_t char_size = (orig.flags & TSTRING_UTF16)? 2 : 1;
  size_t bytes_required = sizeof(struct TStringInfo) + orig.length + char_size;
  struct TStringInfo *newSh =
      reinterpret_cast<struct TStringInfo *>(malloc(bytes_required));
  tref_init(&newSh->count, 1);
  newSh->external_obj = nullptr;
  newSh->dest = nullptr;

  char *buffer = reinterpret_cast<char *>(newSh + 1);
  std::copy(orig.ptr, orig.ptr + orig.length, buffer);
  if (orig.flags & TSTRING_UTF16) {
    uint16_t *dst = reinterpret_cast<uint16_t *>(buffer);
    dst[orig.length / 2] = u'\0';
  } else {
    buffer[orig.length] = '\0';
  }

  tstr.flags = orig.flags & TSTRING_ENCODING_MASK;
  tstr.length = orig.length;
  tstr.pstrinfo = newSh;
  tstr.ptr = buffer;

  TStringCache *cache = to_string_cache(orig);
  if (cache) {
    tref_inc(&cache->count);
    tstr.cache = orig.cache;
    tstr.flags |= TSTRING_HAS_CACHE;
    ;
  } else {
    tstr.cache = nullptr;
    tstr.flags &= ~TSTRING_HAS_CACHE;
  }
  return tstr;
}

void tstr_drop(struct TString tstr) {
  TStringCache *cache = to_string_cache(tstr);
  if (cache && tref_dec(&cache->count)) free(cache);

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

inline size_t utf8_to_utf16_required(char const *src, size_t len) {
  uint8_t const *data = reinterpret_cast<uint8_t const *>(src);
  size_t pos = 0, units = 0;

  while (pos < len) {
    if (pos + 16 <= len) {
      uint64_t v1, v2;
      std::copy(data + pos, data + pos + 8, reinterpret_cast<uint8_t *>(&v1));
      std::copy(data + pos + 8, data + pos + 16,
                reinterpret_cast<uint8_t *>(&v2));
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
      if (pos + 2 >= len || (data[pos + 1] & 0b11000000) != 0b10000000 ||
          (data[pos + 2] & 0b11000000) != 0b10000000) {
        units += 1;
        pos++;
        continue;
      }
      units += 1;
      pos += 3;
    } else if ((leading_byte & 0b11111000) == 0b11110000) {
      // 4 字节 UTF-8
      if (pos + 3 >= len || (data[pos + 1] & 0b11000000) != 0b10000000 ||
          (data[pos + 2] & 0b11000000) != 0b10000000 ||
          (data[pos + 3] & 0b11000000) != 0b10000000) {
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

inline size_t utf8_to_utf16(char const *input, size_t len, uint16_t *output) {
  uint8_t const *data = reinterpret_cast<uint8_t const *>(input);
  size_t pos = 0;
  uint16_t *start{output};

  while (pos < len) {
    if (pos + 16 <= len) {
      uint64_t v1, v2;
      std::copy(data + pos, data + pos + 8, reinterpret_cast<uint8_t *>(&v1));
      std::copy(data + pos + 8, data + pos + 16,
                reinterpret_cast<uint8_t *>(&v2));
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
      if (pos + 2 >= len || (data[pos + 1] & 0b11000000) != 0b10000000 ||
          (data[pos + 2] & 0b11000000) != 0b10000000) {
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
      if (pos + 3 >= len || (data[pos + 1] & 0b11000000) != 0b10000000 ||
          (data[pos + 2] & 0b11000000) != 0b10000000 ||
          (data[pos + 3] & 0b11000000) != 0b10000000) {
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

inline size_t utf16_to_utf8_required(uint16_t const *src, size_t len) {
  uint16_t const *data = src;
  size_t pos = 0, units = 0;
  while (pos < len) {
    if (pos + 4 <= len) {
      uint64_t v;
      std::copy(data + pos, data + pos + 4, reinterpret_cast<uint8_t *>(&v));
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

inline size_t utf16_to_utf8(uint16_t const *input, size_t len, char *output) {
  uint16_t const *data = input;
  size_t pos = 0;
  char *start{output};
  while (pos < len) {
    if (pos + 4 <= len) {
      uint64_t v;
      std::copy(data + pos, data + pos + 4, reinterpret_cast<uint8_t *>(&v));
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

  char const *src = tstr_buf(utf8_str);
  size_t len = utf8_str.length;

  size_t needed = utf8_to_utf16_required(src, len);

  struct TString result;
  uint16_t *dst = tstr_initialize_utf16(&result, (needed + 1));

  if (!dst)
    return (struct TString){.flags = TSTRING_UTF16,
                            .length = 0,
                            .pstrinfo = nullptr,
                            .ptr = nullptr,
                            .cache = nullptr};

  size_t used_len = utf8_to_utf16(src, len, dst);
  dst[used_len] = u'\0';
  result.flags = TSTRING_UTF16;
  result.length = used_len * 2;
  result.cache = nullptr;
  return result;
}

struct TString tstr_utf16_to_utf8(struct TString utf16_str) {
  if (tstr_encoding(utf16_str) == TSTRING_UTF8) return tstr_dup(utf16_str);

  uint16_t const *src = reinterpret_cast<uint16_t const *>(tstr_buf(utf16_str));
  size_t len = utf16_str.length / 2;

  size_t needed = utf16_to_utf8_required(src, len);

  struct TString result;
  char *dst = tstr_initialize(&result, (needed + 1));

  if (!dst)
    return (struct TString){.flags = TSTRING_UTF8,
                            .length = 0,
                            .pstrinfo = nullptr,
                            .ptr = nullptr,
                            .cache = nullptr};

  size_t used_len = utf16_to_utf8(src, len, dst);
  dst[used_len] = '\0';
  result.flags = TSTRING_UTF8;
  result.length = used_len;
  result.cache = nullptr;
  return result;
}

char const *tstr_generate_utf8_cache(struct TString *utf16_tstr) {
  if (utf16_tstr->flags & TSTRING_UTF8) return utf16_tstr->ptr;
  if (utf16_tstr->flags & TSTRING_HAS_CACHE) return utf16_tstr->cache;
  uint16_t const *src =
      reinterpret_cast<uint16_t const *>(tstr_buf(*utf16_tstr));
  size_t len = utf16_tstr->length / 2;
  size_t needed = utf16_to_utf8_required(src, len);

  size_t bytes_required =
      sizeof(struct TStringCache) + (needed + 1) * sizeof(char);

  struct TStringCache *sh =
      reinterpret_cast<struct TStringCache *>(malloc(bytes_required));
  if (!sh) return nullptr;

  tref_init(&sh->count, 1);
  char *buffer = reinterpret_cast<char *>(sh + 1);
  size_t used_len = utf16_to_utf8(src, len, buffer);
  buffer[used_len] = '\0';
  sh->length = used_len;
  utf16_tstr->cache = buffer;
  utf16_tstr->flags |= TSTRING_HAS_CACHE;

  return buffer;
}

uint16_t const *tstr_generate_utf16_cache(struct TString *utf8_tstr) {
  if (utf8_tstr->flags & TSTRING_UTF16)
    return reinterpret_cast<uint16_t const *>(utf8_tstr->ptr);
  if (utf8_tstr->flags & TSTRING_HAS_CACHE)
    return reinterpret_cast<uint16_t const *>(utf8_tstr->cache);
  char const *src = reinterpret_cast<char const *>(tstr_buf(*utf8_tstr));
  size_t len = utf8_tstr->length;
  size_t needed = utf8_to_utf16_required(src, len);

  size_t bytes_required =
      sizeof(struct TStringCache) + (needed + 1) * sizeof(uint16_t);

  struct TStringCache *sh =
      reinterpret_cast<struct TStringCache *>(malloc(bytes_required));
  if (!sh) return nullptr;

  tref_init(&sh->count, 1);
  uint16_t *buffer = reinterpret_cast<uint16_t *>(sh + 1);
  size_t used_len = utf8_to_utf16(src, len, buffer);
  buffer[used_len] = '\0';
  sh->length = used_len;
  utf8_tstr->cache = reinterpret_cast<char *>(buffer);
  utf8_tstr->flags |= TSTRING_HAS_CACHE;

  return buffer;
}

struct TString tstr_concat_utf8_unsafe(size_t count,
                                       struct TString const *tstr_list) {
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
  tstr.cache = nullptr;
  return tstr;
}

struct TString tstr_concat_utf8(size_t count, struct TString const *tstr_list) {
  int32_t flags = 0;
  for (size_t i = 0; i < count; ++i) {
    flags |= tstr_list[i].flags;
  }
  if ((flags & TSTRING_ENCODING_MASK) == TSTRING_UTF8) {
    return tstr_concat_utf8_unsafe(count, tstr_list);
  }
  return tstr_list[0];
}

struct TString tstr_concat_utf16_unsafe(size_t count,
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
  tstr.cache = nullptr;
  return tstr;
}

struct TString tstr_concat_utf16(size_t count,
                                 struct TString const *tstr_list) {
  int32_t flags = 0;
  for (size_t i = 0; i < count; ++i) {
    flags |= tstr_list[i].flags;
  }
  if ((flags & TSTRING_ENCODING_MASK) == TSTRING_UTF16) {
    return tstr_concat_utf16_unsafe(count, tstr_list);
  }
  return tstr_list[0];
}

struct TString tstr_concat(size_t count, struct TString const *tstr_list) {
  int32_t flags = 0;
  size_t array_len = (count + 3) / 4;
  uint8_t *flags_array = reinterpret_cast<uint8_t *>(malloc(array_len));
  std::fill(flags_array, flags_array + array_len, 0);
  for (size_t i = 0; i < count; ++i) {
    flags |= tstr_list[i].flags;
    uint8_t f = 0;
    if ((tstr_list[i].flags & TSTRING_ENCODING_MASK) == TSTRING_UTF8) f |= 0x1;
    if (tstr_list[i].flags & TSTRING_HAS_CACHE) f |= 0x2;

    size_t byte_idx = i / 4;
    size_t bit_offset = (i % 4) * 2;
    flags_array[byte_idx] |= (f << bit_offset);
  }
  if ((flags & TSTRING_ENCODING_MASK) == TSTRING_UTF8) {
    return tstr_concat_utf8_unsafe(count, tstr_list);
  }

  size_t total_len = 0;
  for (size_t i = 0; i < count; ++i) {
    size_t byte_idx = i / 4;
    size_t bit_offset = (i % 4) * 2;
    uint8_t f = (flags_array[byte_idx] >> bit_offset) & 0x3;

    if (f & 0x1) {
      total_len += tstr_list[i].length;
    } else if (f & 0x2) {
      char const *cache = tstr_list[i].cache;
      uint32_t cache_len = *(reinterpret_cast<uint32_t const *>(cache) - 1);
      total_len += cache_len;
    } else {
      uint16_t const *src =
          reinterpret_cast<uint16_t const *>(tstr_list[i].ptr);
      size_t len = tstr_list[i].length / 2;
      total_len += utf16_to_utf8_required(src, len);
    }
  }

  TString result;
  char *dst = tstr_initialize(&result, total_len + 1);
  char *write_ptr = dst;
  size_t actual_len = 0;
  for (size_t i = 0; i < count; ++i) {
    size_t byte_idx = i / 4;
    size_t bit_offset = (i % 4) * 2;
    uint8_t f = (flags_array[byte_idx] >> bit_offset) & 0x3;

    if (f & 0x1) {
      write_ptr = std::copy(tstr_list[i].ptr,
                            tstr_list[i].ptr + tstr_list[i].length, write_ptr);
      actual_len += tstr_list[i].length;
    } else if (f & 0x2) {
      char const *cache = tstr_list[i].cache;
      uint32_t cache_len = *(reinterpret_cast<uint32_t const *>(cache) - 1);
      write_ptr = std::copy(cache, cache + cache_len, write_ptr);
      actual_len += cache_len;
    } else {
      uint16_t const *src =
          reinterpret_cast<uint16_t const *>(tstr_list[i].ptr);
      size_t len = tstr_list[i].length / 2;
      size_t written = utf16_to_utf8(src, len, write_ptr);
      write_ptr += written;
      actual_len += written;
    }
  }
  *write_ptr = '\0';
  result.length = total_len;
  result.cache = nullptr;
}

TH_INLINE struct TString tstr_substr_utf8_unsafe(struct TString tstr,
                                                 size_t pos, size_t len) {
  if (pos > tstr.length) {
    len = 0;
  } else if (pos + len > tstr.length) {
    len = tstr.length - pos;
  }
  return tstr_new_ref(tstr.ptr + pos, len);
}

struct TString tstr_substr_utf8(struct TString tstr, size_t pos, size_t len) {
  if ((tstr.flags & TSTRING_ENCODING_MASK) != TSTRING_UTF8) return tstr;
  return tstr_substr_utf8_unsafe(tstr, pos, len);
}

TH_INLINE struct TString tstr_substr_utf16_unsafe(struct TString tstr,
                                                  size_t pos, size_t len) {
  if (pos > tstr.length / 2) {
    len = 0;
  } else if (pos + len > tstr.length / 2) {
    len = tstr.length / 2 - pos;
  }
  return tstr_new_ref_utf16(reinterpret_cast<uint16_t const *>(tstr.ptr) + pos,
                            len);
}

struct TString tstr_substr_utf16(struct TString tstr, size_t pos, size_t len) {
  if ((tstr.flags & TSTRING_ENCODING_MASK) != TSTRING_UTF16) return tstr;
  return tstr_substr_utf16_unsafe(tstr, pos, len);
}

struct TString tstr_substr(struct TString *tstr, size_t pos, size_t len) {
  if ((tstr->flags & TSTRING_ENCODING_MASK) == TSTRING_UTF8) {
    return tstr_substr_utf8_unsafe(*tstr, pos, len);
  }
  char const *buffer = tstr_generate_utf8_cache(tstr);
  uint32_t cache_len = *(reinterpret_cast<uint32_t const *>(buffer) - 1);
  if (pos > cache_len) {
    len = 0;
  } else if (pos + len > cache_len) {
    len = cache_len - pos;
  }
  return tstr_new_ref(buffer + pos, len);
}
