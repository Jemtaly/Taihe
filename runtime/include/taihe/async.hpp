#pragma once

#include <taihe/async.abi.h>
#include <taihe/common.hpp>
#include "taihe/common.h"

namespace taihe {
template<typename Result>
struct async_context {
  union ResultBuffer {
    Result result;

    ResultBuffer() {}

    ~ResultBuffer() {}
  };

  uint32_t ref_count;
  uint32_t flags;

  TAsyncHandlerStorage storage;
  void (*process_handler_ptr)(TAsyncHandlerStorage storage, Result *result_ptr);
  void (*cleanup_handler_ptr)(TAsyncHandlerStorage storage);

  ResultBuffer buffer;

  async_context(async_context const &) = delete;
  async_context &operator=(async_context const &) = delete;
  async_context(async_context &&) = delete;
  async_context &operator=(async_context &&) = delete;

  async_context(uint32_t ref_count)
      : ref_count(ref_count),
        flags(ASYNC_CONTEXT_NONE),
        process_handler_ptr(nullptr),
        cleanup_handler_ptr(nullptr) {}

  bool dec_ref() {
    return __atomic_sub_fetch(&ref_count, 1, __ATOMIC_ACQ_REL) == 0;
  }

  uint32_t set_flags(uint32_t new_flags) {
    return __atomic_or_fetch(&flags, new_flags, __ATOMIC_ACQ_REL);
  }

  void process_handler() {
    process_handler_ptr(storage, &buffer.result);
  }

  void cleanup_handler() {
    cleanup_handler_ptr(storage);
  }

  template<typename... Args>
  void emplace_result(Args &&...args) {
    TH_ASSERT(!(flags & ASYNC_CONTEXT_RESULT_SET),
              "Result is already being set");
    new (&buffer.result) Result(std::forward<Args>(args)...);

    uint32_t old_falgs = set_flags(ASYNC_CONTEXT_RESULT_SET);
    if (old_falgs & ASYNC_CONTEXT_HANDLER_SET) {
      process_handler();
    }
  }

  template<typename SmallConstHandler, typename... Args>
  void emplace_handler(Args &&...args) {
    static_assert(
        sizeof(SmallConstHandler) <= sizeof(TAsyncHandlerStorage::buf),
        "Handler type is too large for small storage");
    TH_ASSERT(!(flags & ASYNC_CONTEXT_HANDLER_SET),
              "Handler is already being set");
    new (&storage.buf) SmallConstHandler(std::forward<Args>(args)...);
    process_handler_ptr = [](TAsyncHandlerStorage storage, Result *result_ptr) {
      reinterpret_cast<SmallConstHandler const *>(&storage.buf)
          ->handle_result(std::forward<Result>(*result_ptr));
    };
    cleanup_handler_ptr = [](TAsyncHandlerStorage storage) {
      reinterpret_cast<SmallConstHandler const *>(&storage.buf)
          ->~SmallConstHandler();
    };

    uint32_t old_falgs = set_flags(ASYNC_CONTEXT_HANDLER_SET);
    if (old_falgs & ASYNC_CONTEXT_RESULT_SET) {
      process_handler();
    }
  }

  template<typename LargeMutableHandler, typename... Args>
  void new_handler(Args &&...args) {
    TH_ASSERT(!(flags & ASYNC_CONTEXT_HANDLER_SET),
              "Handler is already being set");
    storage.ptr = new LargeMutableHandler(std::forward<Args>(args)...);
    process_handler_ptr = [](TAsyncHandlerStorage storage, Result *result_ptr) {
      reinterpret_cast<LargeMutableHandler *>(storage.ptr)
          ->handle_result(std::forward<Result>(*result_ptr));
    };
    cleanup_handler_ptr = [](TAsyncHandlerStorage storage) {
      delete reinterpret_cast<LargeMutableHandler *>(storage.ptr);
    };

    uint32_t old_falgs = set_flags(ASYNC_CONTEXT_HANDLER_SET);
    if (old_falgs & ASYNC_CONTEXT_RESULT_SET) {
      process_handler();
    }
  }

  bool is_ready() const {
    return flags & ASYNC_CONTEXT_RESULT_SET;
  }

  ~async_context() {
    if (flags & ASYNC_CONTEXT_RESULT_SET) {
      buffer.result.~Result();
    }
    if (flags & ASYNC_CONTEXT_HANDLER_SET) {
      cleanup_handler();
    }
  }
};

template<typename Result>
class async_setter;

template<typename Result>
class async_result;

template<typename Result>
std::pair<async_setter<Result>, async_result<Result>> make_async_pair();

template<typename Result>
class async_setter {
  async_context<Result> *ctx;

  async_setter(async_context<Result> *ctx) : ctx(ctx) {}

  friend std::pair<async_setter<Result>, async_result<Result>>
  make_async_pair<Result>();

public:
  async_setter(async_setter const &) = delete;

  async_setter(async_setter &&other) : ctx(other.ctx) {
    other.ctx = nullptr;
  }

  async_setter &operator=(async_setter other) {
    std::swap(this->ctx, other.ctx);
    return *this;
  }

  ~async_setter() {
    if (ctx && ctx->dec_ref()) {
      delete ctx;
    }
  }

  template<typename... Args>
  void emplace_result(Args &&...args) const {
    ctx->emplace_result(std::forward<Args>(args)...);
  }
};

template<typename Result>
class async_result {
  async_context<Result> *ctx;

  async_result(async_context<Result> *ctx) : ctx(ctx) {}

  friend std::pair<async_setter<Result>, async_result<Result>>
  make_async_pair<Result>();

public:
  async_result(async_result const &) = delete;

  async_result(async_result &&other) : ctx(other.ctx) {
    other.ctx = nullptr;
  }

  async_result &operator=(async_result other) {
    std::swap(this->ctx, other.ctx);
    return *this;
  }

  ~async_result() {
    if (ctx && ctx->dec_ref()) {
      delete ctx;
    }
  }

  template<typename SmallConstHandler, typename... Args>
  void emplace_handler(Args &&...args) const {
    ctx->template emplace_handler<SmallConstHandler>(
        std::forward<Args>(args)...);
  }

  template<typename LargeMutableHandler, typename... Args>
  void new_handler(Args &&...args) const {
    ctx->template new_handler<LargeMutableHandler>(std::forward<Args>(args)...);
  }

  bool is_ready() const {
    return ctx->is_ready();
  }
};

template<typename Result>
std::pair<async_setter<Result>, async_result<Result>> make_async_pair() {
  async_context<Result> *ctx = new async_context<Result>(2);
  return {
      async_setter<Result>(ctx),
      async_result<Result>(ctx),
  };
}

template<typename Result>
struct as_abi<async_setter<Result>> {
  using type = TAsyncSetter;
};

template<typename Result>
struct as_abi<async_result<Result>> {
  using type = TAsyncResult;
};

template<typename Result>
struct as_param<async_setter<Result>> {
  using type = async_setter<Result>;
};

template<typename Result>
struct as_param<async_result<Result>> {
  using type = async_result<Result>;
};

template<typename Result>
inline bool operator==(async_setter<Result> lhs, async_setter<Result> rhs) {
  return lhs.ctx == rhs.ctx;
}

template<typename Result>
inline bool operator==(async_result<Result> lhs, async_result<Result> rhs) {
  return lhs.ctx == rhs.ctx;
}
}  // namespace taihe

template<typename Result>
struct std::hash<taihe::async_setter<Result>> {
  std::size_t operator()(taihe::async_setter<Result> val) const {
    return std::hash<void *>()(val.ctx);
  }
};

template<typename Result>
struct std::hash<taihe::async_result<Result>> {
  std::size_t operator()(taihe::async_result<Result> val) const {
    return std::hash<void *>()(val.ctx);
  }
};

// Utils

#include <condition_variable>
#include <mutex>
#include <optional>

namespace taihe {
template<typename Result>
Result join(async_result<Result> gotter) {
  struct JoinContext {
    std::mutex mtx;
    std::condition_variable cv;
    std::optional<Result> joined;
  };

  struct JoinHandler {
    JoinContext &ctx;

    JoinHandler(JoinContext &ctx) : ctx(ctx) {}

    void handle_result(Result &&result) const {
      std::unique_lock<std::mutex> lock(ctx.mtx);
      ctx.joined.emplace(std::forward<Result>(result));
      ctx.cv.notify_all();
    }
  };

  JoinContext ctx;
  gotter.template emplace_handler<JoinHandler>(ctx);

  std::unique_lock<std::mutex> lock(ctx.mtx);
  ctx.cv.wait(lock, [&ctx]() {
    return ctx.joined.has_value();
  });

  return std::move(ctx.joined.value());
}

template<typename Result>
Result select(std::initializer_list<async_result<Result>> gotters) {
  struct SelectContext {
    std::mutex mtx;
    std::condition_variable cv;
    std::optional<Result> joined;
  };

  struct SelectHandler {
    std::shared_ptr<SelectContext> ptr;

    SelectHandler(std::shared_ptr<SelectContext> ptr) : ptr(ptr) {}

    void handle_result(Result &&result) const {
      std::unique_lock<std::mutex> lock(ptr->mtx);
      if (!ptr->joined.has_value()) {
        ptr->joined.emplace(std::forward<Result>(result));
        ptr->cv.notify_all();
      }
    }
  };

  auto ptr = std::make_shared<SelectContext>();
  for (auto const &old : gotters) {
    old.template emplace_handler<SelectHandler>(ptr);
  }

  std::unique_lock<std::mutex> lock(ptr->mtx);
  ptr->cv.wait(lock, [ptr = ptr.get()]() {
    return ptr->joined.has_value();
  });

  return std::move(ptr->joined.value());
}
}  // namespace taihe

#include <type_traits>

namespace taihe {
template<typename Next, typename Last, typename Processor>
async_result<Next> then(async_result<Last> gotter, Processor &&processor) {
  auto [setter, getter] = make_async_pair<Next>();

  struct ProcessHandler {
    Processor processor;
    async_setter<Next> setter;

    ProcessHandler(Processor &&processor, async_setter<Next> setter)
        : processor(std::forward<Processor>(processor)),
          setter(std::move(setter)) {}

    struct SetterAsHandler {
      async_setter<Next> setter;

      SetterAsHandler(async_setter<Next> setter) : setter(std::move(setter)) {}

      void handle_result(Next &&result) const {
        setter.emplace_result(std::forward<Next>(result));
      }
    };

    void handle_result(Last &&result) {
      if constexpr (std::is_invocable_r_v<async_result<Next>, Processor,
                                          Last>) {
        this->processor(std::forward<Last>(result))
            .template emplace_handler<SetterAsHandler>(std::move(this->setter));
      } else if constexpr (std::is_invocable_v<Processor, Last,
                                               async_setter<Next>>) {
        this->processor(std::forward<Last>(result), std::move(this->setter));
      } else {
        static_assert(false,
                      "Processor cannot handle result without setter or "
                      "promise return type");
      }
    }
  };

  gotter.template new_handler<ProcessHandler>(
      std::forward<Processor>(processor), std::move(setter));
  return std::move(getter);
}
}  // namespace taihe
