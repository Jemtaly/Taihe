#include <chrono>
#include <cstdio>
#include <string>
#include <vector>

#include "../lib/c_api.h"
#include "../lib/impl.h"

// Computes the average execution duration (nano seconds) for "char
// *OH_Concat(const char **args)"
unsigned benchmark(unsigned num_strs, unsigned length_per_str,
                   unsigned num_trials) {
  std::vector<std::string> args;
  std::vector<const char *> argp;

  for (unsigned i = 0; i < num_strs; ++i) {
    std::string str(length_per_str, 'x');
    args.push_back(std::move(str));
    argp.push_back(args.back().c_str());
  }
  argp.push_back(nullptr);

  auto start = std::chrono::high_resolution_clock::now();
  for (unsigned i = 0; i < num_trials; ++i) free(OH_Concat(argp.data()));
  auto end = std::chrono::high_resolution_clock::now();

  auto duration =
      std::chrono::duration_cast<std::chrono::microseconds>(end - start);
  return duration.count() / num_trials;
}

// Computes the average execution duration (nano seconds) for "char
// *OH_Concat(const char **args)"
unsigned benchmark_cpp(unsigned num_strs, unsigned length_per_str,
                       unsigned num_trials) {
  std::vector<std::string> args;

  for (unsigned i = 0; i < num_strs; ++i) {
    std::string str(length_per_str, 'x');
    args.push_back(std::move(str));
  }

  auto start = std::chrono::high_resolution_clock::now();
  for (unsigned i = 0; i < num_trials; ++i) api_impl::concat(args);
  auto end = std::chrono::high_resolution_clock::now();

  auto duration =
      std::chrono::duration_cast<std::chrono::microseconds>(end - start);
  return duration.count() / num_trials;
}

void run_once(unsigned num_strs, unsigned length_per_str, unsigned num_trials) {
  printf("tbench:c-capi,%u,%u,%u\n", num_strs, length_per_str,
         benchmark(num_strs, length_per_str, num_trials));
}

void run_cpp_once(unsigned num_strs, unsigned length_per_str,
                  unsigned num_trials) {
  printf("tbench:cpp-cpp,%u,%u,%u\n", num_strs, length_per_str,
         benchmark_cpp(num_strs, length_per_str, num_trials));
}

int main() {
  run_once(1, 1, 3);
  for (unsigned i = 0; i < 5000; i += 200) {
    run_once(1500, i, 3);
  }

  run_cpp_once(1, 1, 3);
  for (unsigned i = 0; i < 5000; i += 200) {
    run_cpp_once(1500, i, 3);
  }
  return 0;
}
