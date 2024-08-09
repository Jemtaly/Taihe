#include <chrono>
#include <cstdio>
#include <string>
#include <vector>

#include "../lib/c_api.h"
#include "../lib/impl.h"

// Computes the average execution duration (nano seconds) for "char
// *OH_Concat(const char **args)"
unsigned benchmark_c(unsigned num_strs, unsigned length_per_str,
                     unsigned num_trials) {
  std::vector<std::string> args;
  std::vector<const char *> argp;

  for (unsigned i = 0; i < num_strs; ++i)
    args.emplace_back(length_per_str, 'x');
  for (unsigned i = 0; i < num_strs; ++i) argp.emplace_back(args[i].c_str());
  argp.push_back(nullptr);

  auto start = std::chrono::high_resolution_clock::now();
  for (unsigned i = 0; i < num_trials; ++i) free(OH_Concat(argp.data()));
  auto end = std::chrono::high_resolution_clock::now();

  auto duration =
      std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
  return duration.count() / num_trials;
}

unsigned benchmark_cpp(unsigned num_strs, unsigned length_per_str,
                       unsigned num_trials) {
  std::vector<std::string> args;
  for (unsigned i = 0; i < num_strs; ++i)
    args.emplace_back(length_per_str, 'x');

  auto start = std::chrono::high_resolution_clock::now();
  for (unsigned i = 0; i < num_trials; ++i) api_impl::concat(args);
  auto end = std::chrono::high_resolution_clock::now();

  auto duration =
      std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
  return duration.count() / num_trials;
}

unsigned benchmark_taihe(unsigned num_strs, unsigned length_per_str,
                         unsigned num_trials) {
  taihe::TList *ss = new taihe::TList();
  ss->reserve(num_strs);

  for (unsigned i = 0; i < num_strs; ++i) {
    std::string str(length_per_str, 'x');
    auto tstr = taihe::make_sptr<taihe::TString>(std::move(str));
    ss->emplace_back(tstr.cast<taihe::TObject>());
  }

  auto start = std::chrono::high_resolution_clock::now();
  for (unsigned i = 0; i < num_trials; ++i) api_taihe_impl::concat(ss)->unref();
  auto end = std::chrono::high_resolution_clock::now();
  ss->unref();

  auto duration =
      std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
  return duration.count() / num_trials;
}

void run_c_once(unsigned num_strs, unsigned length_per_str,
                unsigned num_trials) {
  printf("tbench:c-capi,%u,%u,%u\n", num_strs, length_per_str,
         benchmark_c(num_strs, length_per_str, num_trials));
}

void run_cpp_once(unsigned num_strs, unsigned length_per_str,
                  unsigned num_trials) {
  printf("tbench:cpp-cpp,%u,%u,%u\n", num_strs, length_per_str,
         benchmark_cpp(num_strs, length_per_str, num_trials));
}

void run_taihe_once(unsigned num_strs, unsigned length_per_str,
                    unsigned num_trials) {
  printf("tbench:cpp-taihe,%u,%u,%u\n", num_strs, length_per_str,
         benchmark_taihe(num_strs, length_per_str, num_trials));
}

int main() {
  run_c_once(0, 1, 100000);
  for (unsigned i = 0; i < 5000; i += 200) {
    run_c_once(1500, i, 3);
  }

  run_cpp_once(0, 1, 100000);
  for (unsigned i = 0; i < 5000; i += 200) {
    run_cpp_once(1500, i, 3);
  }

  run_taihe_once(0, 1, 100000);
  for (unsigned i = 0; i < 5000; i += 200) {
    run_taihe_once(1500, i, 3);
  }
  return 0;
}
