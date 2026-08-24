project(math-simd LANGUAGES CXX)

add_library(simd_common STATIC)
target_compile_features(simd_common PRIVATE cxx_std_26 INTERFACE cxx_std_26)
target_sources(simd_common
        PUBLIC
        FILE_SET module_list TYPE CXX_MODULES FILES
        common.cppm)
target_link_libraries(simd_common PRIVATE math_types)

set(SIMD_ARCH_TARGETS)

macro(decl_arch_taget)
    add_library(${ARGV0} STATIC)
    target_compile_features(${ARGV0} PRIVATE cxx_std_26 INTERFACE cxx_std_26)
    target_compile_options(${ARGV0} PRIVATE -march=${ARGV1} -ffast-math)
    target_sources(${ARGV0}
            PRIVATE
            ${ARGV0}/${ARGV0}.hpp
            ${ARGV0}/${ARGV0}.cpp
            PUBLIC
            FILE_SET module_list TYPE CXX_MODULES FILES
            ${ARGV0}/${ARGV0}.cppm)
    target_include_directories(${ARGV0} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/${ARGV0})
    target_link_libraries(${ARGV0} PRIVATE simd_common math_exception)
    list(APPEND SIMD_ARCH_TARGETS ${ARGV0})
endmacro()

decl_arch_taget(x86_64_v2 x86-64-v2)

decl_arch_taget(x86_64_v4 x86-64-v4)

add_library(fallback STATIC)
target_compile_features(fallback PRIVATE cxx_std_26 INTERFACE cxx_std_26)
target_sources(fallback PUBLIC FILE_SET module_list TYPE CXX_MODULES FILES fallback.cppm)
target_link_libraries(fallback PRIVATE simd_common math_exception)

add_library(simd STATIC)
target_compile_features(simd PRIVATE cxx_std_26 INTERFACE cxx_std_26)
target_sources(simd
        PUBLIC
        FILE_SET module_list TYPE CXX_MODULES FILES
        simd.cppm)
target_link_libraries(simd PRIVATE simd_common fallback ${SIMD_ARCH_TARGETS})
