project(math-types)

add_library(math_types STATIC)
target_compile_features(math_types PRIVATE cxx_std_26 INTERFACE cxx_std_26)
target_sources(math_types
        PUBLIC
        FILE_SET module_list TYPE CXX_MODULES FILES
        matrix.cppm
        types.cppm
        utility.cppm
        vector.cppm)
