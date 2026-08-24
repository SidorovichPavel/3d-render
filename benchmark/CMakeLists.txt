project(any-benchmark LANGUAGES CXX)

macro(declare_benchmark)
    add_executable(${ARGV0} ${ARGV0}.cpp)
    target_compile_features(${ARGV0} PRIVATE cxx_std_26)
    target_link_libraries(${ARGV0} PRIVATE math simd benchmark::benchmark)
endmacro()

declare_benchmark(bench_transpose)
declare_benchmark(bench_matrix_multiplies)
# declare_benchmark(bench_matrix_multiplies_avx512)
# target_link_libraries(bench_matrix_multiplies_avx512 PRIVATE x86_64_v4)