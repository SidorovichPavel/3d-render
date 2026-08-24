#include <benchmark/benchmark.h>

import math;

namespace {

void BM_TransposeMat4x4(benchmark::State& state) {
  const math::mat4 mat4(2.f);
  for (auto _ : state) {
    auto result = math::transpose(mat4);
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK(BM_TransposeMat4x4);

void BM_Transpose2Mat4x4(benchmark::State& state) {
  const math::simd::mat4 mat4(2.f);
  for (auto _ : state) {
    auto result = math::simd::transpose2(mat4);
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK(BM_Transpose2Mat4x4);

}  // namespace

BENCHMARK_MAIN();