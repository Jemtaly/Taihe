#pragma once

#include <taihe/common.h>

union TAsyncHandlerStorage {
  void *ptr;
  char buf[sizeof(void *) * 4];
};

enum TAsyncContextFlags : uint32_t {
  ASYNC_CONTEXT_NONE = 0,
  ASYNC_CONTEXT_RESULT_SET = 1 << 0,
  ASYNC_CONTEXT_HANDLER_SET = 1 << 1,
};

struct TAsyncContext {
  uint32_t ref_count;
  uint32_t flags;

  union TAsyncHandlerStorage storage;
  void (*process_handler_ptr)(union TAsyncHandlerStorage storage, void *resptr);
  void (*cleanup_handler_ptr)(union TAsyncHandlerStorage storage);

  char buffer[];
};

struct TAsyncSetter {
  struct TAsyncContext *ctx;
};

struct TAsyncResult {
  struct TAsyncContext *ctx;
};
