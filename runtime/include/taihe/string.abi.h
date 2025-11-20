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
  TSTRING_UTF8 = 1u << 16,
  TSTRING_UTF16 = 1u << 17,
  TSTRING_UTF32 = 1u << 18,
};

struct TStringInfo {
  TRefCount count;
  void *external_obj;
  void (*dest)(void *);
};

struct TString {
  uint32_t flags;
  uint32_t length;
  struct TStringInfo *pstrinfo;
  char const *ptr;
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

// Allocates memory and initializes a UTF32 encoding TString with a given
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
TH_EXPORT uint32_t *tstr_initialize_utf32(struct TString *tstr_ptr,
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

// Creates a new heap-allocated TString by copying an existing UTF32 string.
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
TH_EXPORT struct TString tstr_new_utf32(uint16_t const *buf TH_NONNULL,
                                        size_t len);

// Creates a TString from an existing UTF32 string.
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
TH_EXPORT struct TString tstr_new_ref_utf32(uint16_t const *buf TH_NONNULL,
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

// Concatenates UTF8 encoding TString objects.
//
// # Parameters
// - `count`: The number of strings to concatenate.
// - `tstr_list`: An array of TString objects to concatenate.
//
// # Returns
// - A new TString object containing the concatenated result.
//
// # Notes
// - The returned TString must be freed using `tstr_drop`.
TH_EXPORT struct TString tstr_concat_utf8(size_t count,
                                          struct TString const *tstr_list);

// Concatenates UTF16 encoding TString objects.
//
// # Parameters
// - `count`: The number of strings to concatenate.
// - `tstr_list`: An array of TString objects to concatenate.
//
// # Returns
// - A new TString object containing the concatenated result.
//
// # Notes
// - The returned TString must be freed using `tstr_drop`.
TH_EXPORT struct TString tstr_concat_utf16(size_t count,
                                           struct TString const *tstr_list);

// Concatenates UTF32 encoding TString objects.
//
// # Parameters
// - `count`: The number of strings to concatenate.
// - `tstr_list`: An array of TString objects to concatenate.
//
// # Returns
// - A new TString object containing the concatenated result.
//
// # Notes
// - The returned TString must be freed using `tstr_drop`.
TH_EXPORT struct TString tstr_concat_utf32(size_t count,
                                           struct TString const *tstr_list);

// Concatenates TString objects.
//
// # Parameters
// - `count`: The number of strings to concatenate.
// - `tstr_list`: An array of TString objects to concatenate.
//
// # Returns
// - A new TString object containing the concatenated result.
//
// # Notes
// - The returned TString must be freed using `tstr_drop`.
TH_EXPORT struct TString tstr_concat(size_t count,
                                     struct TString const *tstr_list);

// Extracts a substring from a UTF8 encoding TString object.
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
TH_EXPORT struct TString tstr_substr_utf8(struct TString tstr, size_t pos,
                                          size_t len);

// Extracts a substring from a UTF16 encoding TString object.
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
TH_EXPORT struct TString tstr_substr_utf16(struct TString tstr, size_t pos,
                                           size_t len);

// Extracts a substring from a UTF32 encoding TString object.
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
TH_EXPORT struct TString tstr_substr_utf32(struct TString tstr, size_t pos,
                                           size_t len);

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
TH_EXPORT struct TString tstr_substr(struct TString tstr, size_t pos,
                                     size_t len);

TH_EXPORT uint32_t tstr_encoding(struct TString tstr);