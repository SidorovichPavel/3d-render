#include <benchmark/benchmark.h>

#include <ranges>

import math;
import math.simd;

namespace {

constexpr std::size_t N[] = {16, 256};

void BM_multiply_mat4_x_mat4(benchmark::State& state) {
  const math::mat4 A{14, 6, 5, 9, 3, 1, 7, 11, 10, 8, 12, 4, 1, 2, 6, 19};
  const math::mat4 B{12, 6, 9, 4, 7, 2, 11, 10, 15, 2, 9, 4, 1, 11, 9, 3};

  for (auto _ : state) {
    auto result = A * B;
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK(BM_multiply_mat4_x_mat4);

void BM_multiply_mat4_x_mat4_2(benchmark::State& state) {
  const math::simd::mat4 A{14, 6, 5, 9, 3, 1, 7, 11, 10, 8, 12, 4, 1, 2, 6, 19};
  const math::simd::mat4 B{12, 6, 9, 4, 7, 2, 11, 10, 15, 2, 9, 4, 1, 11, 9, 3};

  for (auto _ : state) {
    auto result = A * B;
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK(BM_multiply_mat4_x_mat4_2);

}  // namespace

BENCHMARK_MAIN();
