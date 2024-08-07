#pragma once
#include <atomic>
#include <cassert>
#include <memory>
#include <string>
#include <vector>

#define DISALLOW_COPY_AND_ASSIGN(ClassName) \
  ClassName(const ClassName &);             \
  void operator=(const ClassName &);

#define DISALLOW_ALLOCATION()                                      \
 public:                                                           \
  void operator delete(void *pointer) { __builtin_unreachable(); } \
                                                                   \
 private:                                                          \
  void *operator new(size_t size);

namespace taihe {

template <class T>
class sptr;

struct TKlass {
  void (*dtor)(void *self);
};

struct TObject {
  using CountT = uint32_t;

  const TKlass *klass;
  std::atomic<CountT> ref_count;

  TObject(const TKlass *klass) : klass(klass), ref_count(1) {}

  void ref() {
    CountT prev = ref_count.fetch_add(1u, std::memory_order_relaxed);
    assert(prev > 0);
  }

  void unref() {
    CountT prev = ref_count.fetch_sub(1u, std::memory_order_acq_rel);
    assert(prev > 0);
    if (prev == 1) klass->dtor(this);
  }
};

template <class T>
class sptr {
 public:
  sptr() : p(nullptr) {}

  explicit sptr(T *t) : p(t) {
    if (p) p->ref();
  }

  static sptr<T> claim(T *t) { return sptr(t, true); }

  ~sptr() {
    if (p) p->unref();
  }

  void set(T *t) {
    if (p) p->unref();
    p = t;
    if (p) p->ref();
  }

  T *get() const { return p; }
  T &operator*() const { return *p; }
  T *operator->() const { return p; }

  template <typename U>
  U *cast() const {
    return static_cast<U *>(p);
  }

  operator bool() { return p; }

 private:
  T *p;

  sptr(T *t, bool unused) : p(t) { unused = unused; }
};

template <typename T, typename... Args>
inline sptr<T> make_sptr(Args &&...args) {
  return sptr<T>::claim(new T(std::forward<Args>(args)...));
}

class TString : public TObject, public std::string {
 public:
  TString(std::string &&s);

 private:
  static void dtor(void *self);
  static TKlass Klass;
};

// TODO: generics and ABI.
class TList : public TObject, public std::vector<sptr<TObject>> {
 public:
  TList();

 private:
  static void dtor(void *self);
  static TKlass Klass;
};

}  // namespace taihe
