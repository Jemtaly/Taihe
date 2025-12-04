#pragma once

#include <taihe/common.h>

#include <stddef.h>
#include <stdint.h>

/////////////////////////////////////////
// Private ABI: Don't use in your code //
/////////////////////////////////////////

enum TStringFlags {
  TSTRING_MODE_MASK = 0xFFFF,
  TSTRING_ENCODING_MASK = 0xFFFF0000,
  TSTRING_REF = 1u,
  TSTRING_EXT = 1u << 1,
  TSTRING_HAS_CACHE = 1u << 2,
  TSTRING_UTF8 = 1u << 16,
  TSTRING_UTF16 = 1u << 17,
};

struct TStringInfo {
  TRefCount count;
  void *external_obj;
  void (*dest)(void *);
};

struct TStringCache {
  TRefCount count;
  uint32_t length;
  char buffer[];
};

struct TString {
  uint32_t flags;
  uint32_t length;
  struct TStringInfo *pstrinfo;
  char const *ptr;
  char const *cache;
};

//////////////////
// Public C API //
//////////////////

// Returns the buffer of the TString.
TH_INLINE const char *tstr_buf(struct TString tstr) {
  return tstr.ptr;
}

// Returns the length of the TString.
TH_INLINE size_t tstr_len(struct TString tstr) {
  return tstr.length;
}

TH_INLINE const char *tstr_cache(struct TString tstr) {
  return tstr.cache;
}

TH_INLINE size_t tstr_cache_len(struct TString tstr) {
  return *(reinterpret_cast<uint32_t const *>(tstr.cache) - 1);
}

// Allocates memory and initializes a UTF8 encoding TString with a given
// capacity.
//
// # Arguments
// - `tstr_ptr`: Pointer to an uninitialized TString structure.
// - `capacity`: The desired capacity of the string buffer.
//
// # Returns
// - Pointer to the allocated buffer.
//
// # Notes
// - The caller is responsible for setting the string length.
// - Reference count is set to 1 after called.
TH_EXPORT char *tstr_initialize(struct TString *tstr_ptr, uint32_t capacity);

// Allocates memory and initializes a UTF16 encoding TString with a given
// capacity.
//
// # Arguments
// - `tstr_ptr`: Pointer to an uninitialized TString structure.
// - `capacity`: The desired capacity of the string buffer.
//
// # Returns
// - Pointer to the allocated buffer.
//
// # Notes
// - The caller is responsible for setting the string length.
// - Reference count is set to 1 after called.
TH_EXPORT uint16_t *tstr_initialize_utf16(struct TString *tstr_ptr,
                                          uint32_t capacity);

// Creates a new heap-allocated TString by copying an existing UTF8 string.
//
// # Arguments
// - `buf`: A null-terminated string (must not be null).
// - `len`: The length of the string.
//
// # Returns
// - A new TString containing a copy of `buf`.
//
// # Notes
// - The returned TString must be freed using `tstr_drop`.
TH_EXPORT struct TString tstr_new(char const *buf TH_NONNULL, size_t len);

// Creates a TString from an existing UTF8 string.
//
// # Arguments
// - `buf`: a null-terminated string. Null pointer is invalid.
// - `len`: the length of the string.
// - `tstr`: pointer to an uninitialized TString. Do not pass an
//    already-initialized TString here.
//
// # Returns
// - `tstr`, if the string is created successfully. The caller must ensure the
//    string buffer and the returned TString remain unchanged during the whole
//    lifetime of the TString.
// - `NULL`, if the string is not null-terminated, or the length is too large.
//    In this case, the original `tstr` is still uninitialized and should not be
//    used.
TH_EXPORT struct TString tstr_new_ref(char const *buf TH_NONNULL, size_t len);

// Creates a new heap-allocated TString by copying an existing UTF16 string.
//
// # Arguments
// - `buf`: A null-terminated string (must not be null).
// - `len`: The length of the string.
//
// # Returns
// - A new TString containing a copy of `buf`.
//
// # Notes
// - The returned TString must be freed using `tstr_drop`.
TH_EXPORT struct TString tstr_new_utf16(uint16_t const *buf TH_NONNULL,
                                        size_t len);

// Creates a TString from an existing UTF16 string.
//
// # Arguments
// - `buf`: a null-terminated string. Null pointer is invalid.
// - `len`: the length of the string.
// - `tstr`: pointer to an uninitialized TString. Do not pass an
//    already-initialized TString here.
//
// # Returns
// - `tstr`, if the string is created successfully. The caller must ensure the
//    string buffer and the returned TString remain unchanged during the whole
//    lifetime of the TString.
// - `NULL`, if the string is not null-terminated, or the length is too large.
//    In this case, the original `tstr` is still uninitialized and should not be
//    used.
TH_EXPORT struct TString tstr_new_ref_utf16(uint16_t const *buf TH_NONNULL,
                                            size_t len);

// Frees a TString, releasing allocated memory if applicable.
//
// # Arguments
// - `tstr`: The TString to be freed.
//
// # Notes
// - The TString should not be accessed after calling this function.
TH_EXPORT void tstr_drop(struct TString tstr);

// Creates a duplicate of a TString.
//
// # Arguments
// - `tstr`: The TString to be copied.
//
// # Returns
// - A new TString that is either a reference or a deep copy.
//
// # Notes
// - If `tstr` is heap-allocated, its reference count is incremented.
// - If `tstr` is a reference, a new heap-allocated copy is created.
// - Use `tstr_drop` to free the duplicate when done.
TH_EXPORT struct TString tstr_dup(struct TString tstr);

// Concatenates multiple TString objects into a single TString.
//
// This function automatically handles UTF-8 and UTF-16 encoded strings.
// For each input TString:
// - If it is UTF-8, its contents are copied directly.
// - If it is UTF-16 but has a UTF-8 cache, the cached UTF-8 string is used.
// - Otherwise, UTF-16 strings are converted to UTF-8 during concatenation.
//
// # Parameters
// - `count`: The number of strings in `tstr_list`.
// - `tstr_list`: Pointer to an array of `TString` objects to concatenate.
//
// # Returns
// - A new `TString` object in UTF-8 encoding containing the concatenated
// result.
// - The returned TString has owned memory and must be freed using `tstr_drop`.
//
// # Notes
// - This function is "unchecked" in the sense that it assumes valid input
//   encodings. It does not perform strict UTF validation and may replace
//   invalid sequences with replacement characters (U+FFFD) in UTF-16 to UTF-8
//   conversion.
// - Caches in input TStrings are not modified; however, if a UTF-16 input
// string
//   has no UTF-8 cache, a temporary UTF-8 buffer may be created internally
//   during concatenation.
TH_EXPORT struct TString tstr_concat(size_t count,
                                     struct TString const *tstr_list);

// Extracts a substring from a TString object.
//
// # Parameters
// - `tstr`: The source TString object to extract the substring from.
// - `pos`: The starting position of the substring within the source TString
//   object.
// - `len`: The length of the substring to extract.
//
// # Returns
// - A TString reference of the extracted substring.
//
// # Notes
// - The returned TString is just a view of the original string and does not own
//   the memory, so it should not be freed.
TH_EXPORT struct TString tstr_substr(struct TString *tstr, size_t pos,
                                     size_t len);

// Retrieves the encoding type of a TString.
//
// # Parameters
// - `tstr`: The TString whose encoding type is to be retrieved.
//
// # Returns
// - A `uint32_t` value representing the encoding of the TString.
//   user can compare with TSTRING_UTF8 / TSTRING_UTF16
//   Possible values typically correspond to UTF-8, UTF-16, or UTF-32.
//
// # Notes
// - This function does not modify the TString.
TH_EXPORT uint32_t tstr_encoding(struct TString tstr);

// Converts a UTF8-encoded TString object into a UTF16-encoded TString.
//
// # Parameters
// - `utf8_str`: The source TString encoded in UTF8.
//
// # Returns
// - A new TString encoded in UTF16.
//   The returned TString owns its memory and must be freed with `tstr_drop`.
//
// # Notes
// - Invalid UTF8 sequences are handled according to the internal conversion
//   policy (typically replacing invalid sequences with U+FFFD).
// - Serious errors return an empty string U'\0'.
// - The returned TString is heap-allocated and independent of the input.
TH_EXPORT struct TString tstr_utf8_to_utf16(struct TString utf8_str);

// Converts a UTF16-encoded TString object into a UTF8-encoded TString.
//
// # Parameters
// - `utf16_str`: The source TString encoded in UTF16.
//
// # Returns
// - A new TString encoded in UTF8.
//   The returned TString owns its memory and must be freed with `tstr_drop`.
//
// # Notes
// - Invalid surrogate pairs or malformed UTF16 sequences are handled according
//   to the internal conversion policy (typically replacing invalid sequences
//   with U+FFFD).
// - The returned TString is heap-allocated and independent of the input.
TH_EXPORT struct TString tstr_utf16_to_utf8(struct TString utf16_str);

// Generates (or retrieves) the cached UTF-8 representation of a UTF-16 TString.
//
// # Parameters
// - `utf16_tstr`: A pointer to a TString encoded in UTF-16.
//
// # Returns
// - A pointer to a null-terminated UTF-8 byte sequence representing the content
//   of the input TString.
//   *The returned pointer is borrowed and owned by `utf16_tstr`; it must NOT be
//   freed.*
//
// # Notes
// - If this TString is UTF-8 encoding, the funtion simply returns the existing
//   existing pointer.
// - If the UTF-8 cache for this TString has already been created, the function
//   simply returns the existing cached pointer.
// - If no cache exists yet, the function allocates memory inside the TString
//   object, performs UTF-16 → UTF-8 conversion, stores the result as an
//   internal cache, and returns the pointer.
// - The returned memory remains valid until the TString is modified or dropped
//   (freed with `tstr_drop`).
// - The function does not modify the string content itself; it only populates
//   the lazy conversion cache.
TH_EXPORT char const *tstr_generate_utf8_cache(struct TString *utf16_tstr);

// Generates (or retrieves) the cached UTF-16 representation of a UTF-8 TString.
//
// # Parameters
// - `utf8_tstr`: A pointer to a TString encoded in UTF-8.
//
// # Returns
// - A pointer to a null-terminated UTF-16 sequence representing the content
//   of the input TString.
//   *The returned pointer is borrowed and owned by `utf8_tstr`; it must NOT be
//   freed.*
//
// # Notes
// - If this TString is UTF-16 encoding, the funtion simply returns the existing
//   existing pointer.
// - If the UTF-16 cache for this TString has already been created, the function
//   simply returns the existing cached pointer.
// - If no cache exists yet, the function allocates memory inside the TString
//   object, performs UTF-8 → UTF-16 conversion, stores the result as an
//   internal cache, and returns the pointer.
// - The returned memory remains valid until the TString is modified or dropped
//   (freed with `tstr_drop`).
// - The function does not modify the string content itself; it only populates
//   the lazy conversion cache.
TH_EXPORT uint16_t const *tstr_generate_utf16_cache(struct TString *utf8_tstr);
