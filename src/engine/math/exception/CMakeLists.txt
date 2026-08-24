project(math-except LANGUAGES CXX)

add_library(math_exception STATIC)
target_compile_features(math_exception PRIVATE cxx_std_26 INTERFACE cxx_std_26)
target_sources(math_exception
        PUBLIC
        FILE_SET module_list TYPE CXX_MODULES FILES
        exception.cppm)
target_link_libraries(math_exception PRIVATE exception)
