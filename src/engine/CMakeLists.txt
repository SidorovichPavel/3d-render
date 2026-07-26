project(engine)

add_library(math OBJECT)
target_compile_features(math PRIVATE cxx_std_26)
target_sources(math
        PUBLIC
        FILE_SET module_list TYPE CXX_MODULES FILES
        math/math.cppm
        math/vector.cppm
        math/utility.cppm)
