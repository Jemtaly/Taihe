#include <chrono>
#include <cstdio>
#include <string>
#include <vector>

#include "../lib/c_api.h"

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

int main() {
  benchmark(1, 1, 1);
  for (unsigned i = 0; i < 5000; i += 200) {
    printf("%8u,%8u\n", i, benchmark(1500, i, 3));
  }
  return 0;
}
