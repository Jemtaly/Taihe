#pragma once

#include <taihe/string.abi.h>
#include <taihe/common.hpp>

#include <algorithm>
#include <charconv>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

namespace taihe {
struct string_view;
struct string;
template<typename CharT>
struct basic_string_view;
template<typename CharT>
struct basic_string;

template<typename CharT>
struct encoding_flag;

template<>
struct encoding_flag<char> {
  static constexpr uint32_t value = TSTRING_UTF8;
};

template<>
struct encoding_flag<char16_t> {
  static constexpr uint32_t value = TSTRING_UTF16;
};

template<>
struct encoding_flag<char32_t> {
  static constexpr uint32_t value = TSTRING_UTF32;
};

struct string_view {
  using value_type = char;
  using size_type = std::size_t;
  using const_reference = value_type const &;
  using const_pointer = value_type const *;
  using const_iterator = const_pointer;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  explicit string_view(struct TString handle) : m_handle(handle) {}

  string_view(char const *value TH_NONNULL)
      : string_view(tstr_new_ref(value, std::strlen(value))) {}

  string_view(char const *value TH_NONNULL, size_type size)
      : string_view(tstr_new_ref(value, size)) {}

  string_view(char16_t const *value TH_NONNULL, size_type size)
      : string_view(tstr_new_ref_utf16(
            reinterpret_cast<uint16_t const *>(value), size)) {}

  string_view(std::initializer_list<char> value)
      : string_view(value.begin(), value.size()) {}

  string_view(std::string_view value)
      : string_view(value.data(), value.size()) {}

  string_view(std::string const &value)
      : string_view(value.data(), value.size()) {}

  template<typename CharT>
  explicit string_view(basic_string_view<CharT> const &value) noexcept
      : m_handle(value.m_handle) {}

  operator std::string_view() const noexcept {
    return {tstr_buf(m_handle), tstr_len(m_handle)};
  }

  // methods
  const_reference operator[](size_type pos) const {
    if (pos >= size()) {
      TH_THROW(std::out_of_range, "Index out of range");
    }
    return tstr_buf(m_handle)[pos];
  }

  bool empty() const noexcept {
    return tstr_len(m_handle) == 0;
  }

  size_type size() const noexcept {
    return tstr_len(m_handle);
  }

  const_reference front() const {
    if (empty()) {
      TH_THROW(std::out_of_range, "Empty string");
    }
    return tstr_buf(m_handle)[0];
  }

  const_reference back() const {
    if (empty()) {
      TH_THROW(std::out_of_range, "Empty string");
    }
    return tstr_buf(m_handle)[size() - 1];
  }

  const_pointer c_str() const noexcept {
    return tstr_buf(m_handle);
  }

  const_pointer data() const noexcept {
    return tstr_buf(m_handle);
  }

  const_iterator begin() const noexcept {
    return tstr_buf(m_handle);
  }

  const_iterator cbegin() const noexcept {
    return begin();
  }

  const_iterator end() const noexcept {
    return tstr_buf(m_handle) + tstr_len(m_handle);
  }

  const_iterator cend() const noexcept {
    return end();
  }

  const_reverse_iterator rbegin() const noexcept {
    return const_reverse_iterator(end());
  }

  const_reverse_iterator crbegin() const noexcept {
    return rbegin();
  }

  const_reverse_iterator rend() const noexcept {
    return const_reverse_iterator(begin());
  }

  const_reverse_iterator crend() const noexcept {
    return rend();
  }

  template<typename CharT>
  explicit operator basic_string_view<CharT>() const {
    uint32_t enc = tstr_encoding(m_handle);
    if (enc != encoding_flag<CharT>::value) {
      TH_THROW(std::invalid_argument, "Encoding mismatch in conversion");
    }
    return basic_string_view<CharT>(m_handle);
  }

  friend struct string;
  template<typename CharT>
  friend struct basic_string_view;

  friend string concat(std::initializer_list<string_view> sv_list);
  friend string_view substr(string_view sv, std::size_t pos, std::size_t len);
  friend string operator+(string_view left, string_view right);
  friend uint32_t tstr_encoding(TString const &h);
  template<typename CharT>
  friend basic_string_view<CharT> to_basic_string_view(string_view sv);
  string_view substr(std::size_t pos, std::size_t len) const;

protected:
  struct TString m_handle;
};

struct string : public string_view {
  explicit string(struct TString handle) : string_view(handle) {}

  string(char const *value TH_NONNULL)
      : string(tstr_new(value, std::strlen(value))) {}

  string(char const *value TH_NONNULL, size_type size)
      : string(tstr_new(value, size)) {}

  string(char16_t const *value TH_NONNULL, size_type size)
      : string(
            tstr_new_utf16(reinterpret_cast<uint16_t const *>(value), size)) {}

  string(std::initializer_list<char> value)
      : string(value.begin(), value.size()) {}

  string(std::string_view value) : string(value.data(), value.size()) {}

  string(std::string const &value) : string(value.data(), value.size()) {}

  template<typename CharT>
  explicit string(basic_string<CharT> value) noexcept : string(value.m_handle) {
    value.m_handle.ptr = NULL;
  }

  // constructors
  string(string_view const &other) : string(tstr_dup(other.m_handle)) {}

  string(string const &other) : string(tstr_dup(other.m_handle)) {}

  string(string &&other) noexcept : string(other.m_handle) {
    other.m_handle.ptr = NULL;
  }

  // assignment
  string &operator=(string other) {
    std::swap(this->m_handle, other.m_handle);
    return *this;
  }

  // destructor
  ~string() {
    if (m_handle.ptr != NULL) {
      tstr_drop(m_handle);
    }
  }

  string &operator+=(string_view other);

  template<typename CharT>
  operator basic_string<CharT>() && {
    uint32_t enc = tstr_encoding(this->m_handle);
    if (enc != encoding_flag<CharT>::value) {
      TH_THROW(std::invalid_argument, "Encoding mismatch in conversion");
    }
    basic_string<CharT> res(m_handle);
    this->m_handle.ptr = NULL;
    return res;
  }

  friend uint32_t tstr_encoding(TString const &h);
  template<typename CharT>
  friend basic_string<CharT> to_basic_string(string &&sv);
};

template<typename CharT>
struct basic_string_view {
public:
  using value_type = CharT;
  using size_type = std::size_t;
  using const_reference = value_type const &;
  using const_pointer = value_type const *;

  explicit basic_string_view(TString handle) noexcept : m_handle(handle) {}

  template<typename T = CharT,
           typename = std::enable_if_t<std::is_same_v<T, char>>>
  basic_string_view(std::string_view value) noexcept
      : m_handle(tstr_new_ref(value.data(), value.size())) {}

  template<typename T = CharT,
           typename = std::enable_if_t<std::is_same_v<T, char>>>
  basic_string_view(std::string const &value) noexcept
      : m_handle(tstr_new_ref(value.data(), value.size())) {}

  template<typename T = CharT,
           typename = std::enable_if_t<std::is_same_v<T, char>>>
  operator std::string_view() const noexcept {
    return {tstr_buf(m_handle), tstr_len(m_handle)};
  }

  template<typename T = CharT,
           typename = std::enable_if_t<std::is_same_v<T, char16_t>>>
  basic_string_view(std::u16string_view value) noexcept
      : m_handle(tstr_new_ref_utf16(
            reinterpret_cast<uint16_t const *>(value.data()), value.size())) {}

  template<typename T = CharT,
           typename = std::enable_if_t<std::is_same_v<T, char16_t>>>
  basic_string_view(std::u16string const &value) noexcept
      : m_handle(tstr_new_ref_utf16(
            reinterpret_cast<uint16_t const *>(value.data()), value.size())) {}

  template<typename T = CharT,
           typename = std::enable_if_t<std::is_same_v<T, char16_t>>>
  operator std::u16string_view() const noexcept {
    return {reinterpret_cast<char16_t const *>(tstr_buf(m_handle)),
            tstr_len(m_handle)};
  }

  template<typename T = CharT,
           typename = std::enable_if_t<std::is_same_v<T, char32_t>>>
  basic_string_view(std::u32string_view value) noexcept
      : m_handle(tstr_new_ref_utf32(
            reinterpret_cast<uint16_t const *>(value.data()), value.size())) {}

  template<typename T = CharT,
           typename = std::enable_if_t<std::is_same_v<T, char32_t>>>
  basic_string_view(std::u32string const &value) noexcept
      : m_handle(tstr_new_ref_utf32(
            reinterpret_cast<uint16_t const *>(value.data()), value.size())) {}

  template<typename T = CharT,
           typename = std::enable_if_t<std::is_same_v<T, char32_t>>>
  operator std::u32string_view() const noexcept {
    return {reinterpret_cast<char32_t const *>(tstr_buf(m_handle)),
            tstr_len(m_handle)};
  }

  const_pointer data() const noexcept {
    return reinterpret_cast<const_pointer>(tstr_buf(m_handle));
  }

  size_type size_bytes() const noexcept {
    return tstr_len(m_handle);
  }

  size_type size() const noexcept {
    if constexpr (std::is_same_v<CharT, char>) {
      return tstr_len(m_handle);
    } else if constexpr (std::is_same_v<CharT, char16_t>) {
      return tstr_len(m_handle) / 2;
    } else if constexpr (std::is_same_v<CharT, char32_t>) {
      return tstr_len(m_handle) / 4;
    }
    return tstr_len(m_handle);
  }

  bool empty() const noexcept {
    return size() == 0;
  }

  TString const &handle() const noexcept {
    return m_handle;
  }

  friend struct basic_string<CharT>;
  friend basic_string<CharT> concat(
      std::initializer_list<basic_string_view<CharT>> sv_list);
  friend basic_string<CharT> operator+(basic_string_view<CharT> left,
                                       basic_string_view<CharT> right);
  friend basic_string_view<CharT> substr(basic_string_view<CharT> sv,
                                         std::size_t pos, std::size_t len);
  basic_string_view<CharT> substr(std::size_t pos, std::size_t len) const;

protected:
  TString m_handle;
};

template<typename CharT>
struct basic_string : public basic_string_view<CharT> {
public:
  using value_type = CharT;
  using size_type = std::size_t;
  using const_reference = value_type const &;
  using const_pointer = value_type const *;

  explicit basic_string(struct TString handle)
      : basic_string_view<CharT>(handle) {}

  basic_string(basic_string_view<CharT> const &other)
      : basic_string_view<CharT>(tstr_dup(other.m_handle)) {}

  template<typename T = CharT,
           typename = std::enable_if_t<std::is_same_v<T, char>>>
  basic_string(std::string_view value) noexcept
      : basic_string_view<CharT>(tstr_new(value.data(), value.size())) {}

  template<typename T = CharT,
           typename = std::enable_if_t<std::is_same_v<T, char>>>
  basic_string(std::string const &value) noexcept
      : basic_string_view<CharT>(tstr_new(value.data(), value.size())) {}

  template<typename T = CharT,
           typename = std::enable_if_t<std::is_same_v<T, char16_t>>>
  basic_string(std::u16string_view value) noexcept
      : basic_string_view<CharT>(tstr_new_utf16(
            reinterpret_cast<uint16_t const *>(value.data()), value.size())) {}

  template<typename T = CharT,
           typename = std::enable_if_t<std::is_same_v<T, char16_t>>>
  basic_string(std::u16string const &value) noexcept
      : basic_string_view<CharT>(tstr_new_utf16(
            reinterpret_cast<uint16_t const *>(value.data()), value.size())) {}

  template<typename T = CharT,
           typename = std::enable_if_t<std::is_same_v<T, char32_t>>>
  basic_string(std::u32string_view value) noexcept
      : basic_string_view<CharT>(tstr_new_utf16(
            reinterpret_cast<uint16_t const *>(value.data()), value.size())) {}

  template<typename T = CharT,
           typename = std::enable_if_t<std::is_same_v<T, char32_t>>>
  basic_string(std::u32string const &value) noexcept
      : basic_string_view<CharT>(tstr_new_utf16(
            reinterpret_cast<uint16_t const *>(value.data()), value.size())) {}
};

template<typename CharT>
basic_string_view<CharT> to_basic_string_view(string_view sv) {
  uint32_t enc = tstr_encoding(sv.m_handle);
  if (enc != encoding_flag<CharT>::value) {
    TH_THROW(std::invalid_argument,
             "Encoding mismatch in to_basic_string_view");
  }
  return basic_string_view<CharT>(sv.m_handle);
}

template<typename CharT>
string_view to_string_view(basic_string_view<CharT> bsv) noexcept {
  return string_view(bsv.m_handle);
}

template<typename CharT>
basic_string<CharT> to_basic_string(string &&sv) {
  uint32_t enc = tstr_encoding(sv.m_handle);
  if (enc != encoding_flag<CharT>::value) {
    TH_THROW(std::invalid_argument, "Encoding mismatch in to_basic_string");
  }
  basic_string<CharT> res(sv.m_handle);
  sv.m_handle.ptr = NULL;
  return res;
}

template<typename CharT>
string to_string(basic_string<CharT> &&bs) noexcept {
  string res(bs.m_handle);
  bs.m_handle.ptr = NULL;
  return res;
}

using u8string_view = basic_string_view<char>;
using u8string = basic_string<char>;
using u16string_view = basic_string_view<char16_t>;
using u16string = basic_string<char16_t>;
using u32string_view = basic_string_view<char32_t>;
using u32string = basic_string<char32_t>;

inline string concat(std::initializer_list<string_view> sv_list) {
  static_assert(alignof(string_view) == alignof(struct TString));
  return string(
      tstr_concat(sv_list.size(),
                  reinterpret_cast<struct TString const *>(sv_list.begin())));
}

template<typename CharT>
inline basic_string<CharT> concat(
    std::initializer_list<basic_string_view<CharT>> sv_list) {
  static_assert(alignof(basic_string_view<CharT>) == alignof(struct TString));
  if constexpr (std::is_same_v<CharT, char>) {
    return basic_string<char>(tstr_concat_utf8(
        sv_list.size(),
        reinterpret_cast<struct TString const *>(sv_list.begin())));
  } else if constexpr (std::is_same_v<CharT, char16_t>) {
    return basic_string<char16_t>(tstr_concat_utf16(
        sv_list.size(),
        reinterpret_cast<struct TString const *>(sv_list.begin())));
  } else if constexpr (std::is_same_v<CharT, char32_t>) {
    return basic_string<char32_t>(tstr_concat_utf32(
        sv_list.size(),
        reinterpret_cast<struct TString const *>(sv_list.begin())));
  } else {
    static_assert(always_false<CharT>,
                  "Unsupported character encoding for concat");
  }
}

template<typename CharT>
inline basic_string<CharT> operator+(basic_string_view<CharT> left,
                                     basic_string_view<CharT> right) {
  return concat({left, right});
}

inline string operator+(string_view left, string_view right) {
  return concat({left, right});
}

inline string &string::operator+=(string_view other) {
  return *this = *this + other;
}

inline string_view substr(string_view sv, std::size_t pos, std::size_t len) {
  return string_view(tstr_substr(sv.m_handle, pos, len));
}

inline string_view string_view::substr(std::size_t pos, std::size_t len) const {
  return string_view(tstr_substr(this->m_handle, pos, len));
}

template<typename CharT>
inline basic_string_view<CharT> substr(basic_string_view<CharT> sv,
                                       std::size_t pos, std::size_t len) {
  if constexpr (std::is_same_v<CharT, char>) {
    return basic_string_view<char>(tstr_substr_utf8(sv.m_handle, pos, len));
  } else if constexpr (std::is_same_v<CharT, char16_t>) {
    return basic_string_view<char16_t>(
        tstr_substr_utf16(sv.m_handle, pos, len));
  } else if constexpr (std::is_same_v<CharT, char32_t>) {
    return basic_string_view<char32_t>(
        tstr_substr_utf32(sv.m_handle, pos, len));
  } else {
    static_assert(always_false<CharT>,
                  "Unsupported character encoding for concat");
  }
}

template<typename CharT>
inline basic_string_view<CharT> basic_string_view<CharT>::substr(
    std::size_t pos, std::size_t len) const {
  if constexpr (std::is_same_v<CharT, char>) {
    return basic_string_view<char>(tstr_substr_utf8(this->m_handle, pos, len));
  } else if constexpr (std::is_same_v<CharT, char16_t>) {
    return basic_string_view<char16_t>(
        tstr_substr_utf16(this->m_handle, pos, len));
  } else if constexpr (std::is_same_v<CharT, char32_t>) {
    return basic_string_view<char32_t>(
        tstr_substr_utf32(this->m_handle, pos, len));
  } else {
    static_assert(always_false<CharT>,
                  "Unsupported character encoding for concat");
  }
}

inline bool operator==(string_view lhs, string_view rhs) {
  return std::string_view(lhs) == std::string_view(rhs);
}

inline bool operator!=(string_view lhs, string_view rhs) {
  return std::string_view(lhs) != std::string_view(rhs);
}

inline bool operator<(string_view lhs, string_view rhs) {
  return std::string_view(lhs) < std::string_view(rhs);
}

inline bool operator>(string_view lhs, string_view rhs) {
  return std::string_view(lhs) > std::string_view(rhs);
}

inline bool operator<=(string_view lhs, string_view rhs) {
  return std::string_view(lhs) <= std::string_view(rhs);
}

inline bool operator>=(string_view lhs, string_view rhs) {
  return std::string_view(lhs) >= std::string_view(rhs);
}

inline std::ostream &operator<<(std::ostream &os, string_view sv) {
  return os << std::string_view(sv);
}

template<typename T, std::enable_if_t<std::is_integral_v<T>, int> = 0>
inline string to_string(T value) {
  char buffer[32];
  std::to_chars_result result =
      std::to_chars(std::begin(buffer), std::end(buffer), value);
  if (result.ec != std::errc{}) {
    TH_THROW(std::runtime_error, "Conversion to char failed");
  }
  // buffer automatcally
  return string{buffer, static_cast<std::size_t>(result.ptr - buffer)};
}

template<typename T, std::enable_if_t<std::is_floating_point_v<T>, int> = 0>
inline string to_string(T value) {
  char buffer[32];
  std::to_chars_result result = std::to_chars(
      std::begin(buffer), std::end(buffer), value, std::chars_format::general);
  if (result.ec != std::errc{}) {
    TH_THROW(std::runtime_error, "Conversion to char failed");
  }
  // buffer automatcally
  return string{buffer, static_cast<std::size_t>(result.ptr - buffer)};
}

template<typename T, std::enable_if_t<std::is_same_v<T, bool>, int> = 0>
string to_string(T value) {
  if (value) {
    return string{"true", 4};
  } else {
    return string{"false", 5};
  }
}

template<>
struct as_abi<string_view> {
  using type = TString;
};

template<>
struct as_abi<string> {
  using type = TString;
};

template<>
struct as_param<string> {
  using type = string_view;
};
}  // namespace taihe

template<>
struct std::hash<taihe::string> {
  std::size_t operator()(taihe::string_view sv) const noexcept {
    return std::hash<std::string_view>()(std::string_view(sv));
  }
};
