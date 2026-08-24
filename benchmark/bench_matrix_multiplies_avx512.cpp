#include <benchmark/benchmark.h>

#include <ranges>

import math;
import math.simd;
import math.simd.x86_64_v4;

namespace {

using math::simd::x86_64_v4::mul;

using math::simd::mat4;

auto mul1(const auto& A, const auto& B) { return mul(A, B); }

#define decl_benchmark(NAME, REV, N)                                     \
  void benchmark_##NAME##_rev_##REV##_for_##N(benchmark::State& state) { \
    const mat4 A{14, 6, 5, 9, 3, 1, 7, 11, 10, 8, 12, 4, 1, 2, 6, 19};   \
    const mat4 B{12, 6, 9, 4, 7, 2, 11, 10, 15, 2, 9, 4, 1, 11, 9, 3};   \
    std::array<mat4, N> res;                                             \
                                                                         \
    for (auto _ : state) {                                               \
      for (auto i : std::views::indices(N)) {                            \
        res[i] = mul##REV(A, B);                                         \
      }                                                                  \
    }                                                                    \
    benchmark::DoNotOptimize(res.data());                                \
  }                                                                      \
                                                                         \
  BENCHMARK(benchmark_##NAME##_rev_##REV##_for_##N)

decl_benchmark(mat_mul_avx512, 1, 1);
decl_benchmark(mat_mul_avx512, 1, 10);
decl_benchmark(mat_mul_avx512, 1, 100);
decl_benchmark(mat_mul_avx512, 1, 500);
decl_benchmark(mat_mul_avx512, 1, 1000);
decl_benchmark(mat_mul_avx512, 1, 10000);

}  // namespace

BENCHMARK_MAIN();
