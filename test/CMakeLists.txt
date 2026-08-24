project(any-test LANGUAGES CXX)

enable_testing()

include(GoogleTest)

macro(declare_test)
    add_executable(${ARGV0} ${ARGV0}.cpp)
    target_compile_features(${ARGV0} PRIVATE cxx_std_26)
    target_link_libraries(${ARGV0} math simd GTest::gtest_main)
    gtest_discover_tests(${ARGV0})
endmacro()

declare_test(test_vector)
# declare_test(test_vector_batch)
declare_test(test_matrix)
declare_test(test_math3d)
