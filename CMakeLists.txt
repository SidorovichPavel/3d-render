cmake_minimum_required(VERSION 4.2)

project(3d-render VERSION 0.0.1 LANGUAGES CXX)

include(cmake/CPM.cmake)

# Dependencies
CPMAddPackage("gh:boostorg/boost#boost-1.91.0")
CPMAddPackage("gh:emoon/minifb#v0.10.1")
CPMAddPackage("gh:sreiter/stl_reader#v2.0")
CPMAddPackage("gh:NVIDIA/stdexec#nvhpc-25.09")
CPMAddPackage(URI "gh:gabime/spdlog#v1.16.0"
        OPTIONS
        "SPDLOG_USE_STD_FORMAT ON"
        "SPDLOG_BUILD_WARNINGS ON"
        "SPDLOG_NO_TLS ON")

# Testing
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
CPMAddPackage("gh:google/googletest#v1.17.0")
add_subdirectory(test)

# Benchmark
CPMAddPackage("gh:google/benchmark#v1.9.5")
add_subdirectory(benchmark)

# add_subdirectory(third_party/tinyalgebra)
# add_subdirectory(third_party/threadpool)

add_subdirectory(src/engine)

add_library(exception STATIC)
target_compile_features(exception PRIVATE cxx_std_26 INTERFACE cxx_std_26)
target_link_libraries(exception INTERFACE Boost::stacktrace)

target_sources(exception
        PUBLIC
        FILE_SET module_list TYPE CXX_MODULES FILES
        src/exception.cppm)

add_library(render_obj OBJECT)
target_compile_features(render_obj PRIVATE cxx_std_26 INTERFACE cxx_std_26)
target_compile_options(render_obj PUBLIC -freflection)
target_link_libraries(render_obj PRIVATE
        exception
        universal
        minifb::minifb
        spdlog::spdlog)

target_sources(render_obj
        PUBLIC
        FILE_SET module_list TYPE CXX_MODULES FILES
        src/reflection.cppm
        src/window.cppm)

add_library(universal OBJECT)
target_compile_features(universal PRIVATE cxx_std_26)
target_sources(universal
        PUBLIC
        FILE_SET module_list TYPE CXX_MODULES FILES
        src/universal/color_scheme.cppm
        src/universal/universal.cppm
        src/universal/flags.cppm)

add_executable(${PROJECT_NAME})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_26)

target_sources(${PROJECT_NAME}
        PRIVATE
        src/main.cpp)

target_link_libraries(${PROJECT_NAME}
        PRIVATE
        exception
        Boost::program_options
        render_obj
        spdlog::spdlog
        universal
        stl_reader
        math)
