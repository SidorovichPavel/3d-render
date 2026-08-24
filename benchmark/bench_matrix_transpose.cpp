#include <benchmark/benchmark.h>

#include <ranges>

import math;

namespace {

// void BM_multiply_mat4_x_mat4_3(benchmark::State& state) {
//   const math::simd::mat4 A{14, 6, 5, 9, 3, 1, 7, 11, 10, 8, 12, 4, 1, 2, 6,
//   19}; const math::simd::mat4 B{12, 6, 9, 4, 7, 2, 11, 10, 15, 2, 9, 4, 1,
//   11, 9, 3};
//
//   for (auto _ : state) {
//     auto result = math::mat_mul3(A, B);
//     benchmark::DoNotOptimize(result);
//   }
// }
// BENCHMARK(BM_multiply_mat4_x_mat4_3);

}  // namespace

BENCHMARK_MAIN();
