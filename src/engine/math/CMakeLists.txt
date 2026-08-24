project(math-project LANGUAGES CXX)

add_subdirectory(exception)
add_subdirectory(simd)
add_subdirectory(types)

add_library(math STATIC)
target_compile_features(math PRIVATE cxx_std_26 INTERFACE cxx_std_26)
target_sources(math
        PUBLIC
        FILE_SET module_list TYPE CXX_MODULES FILES
        common.cppm
        math.cppm)

target_link_libraries(math PRIVATE math_exception math_types)
