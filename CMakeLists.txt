cmake_minimum_required(VERSION 3.25.0)
project(3d-render VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(GLEW REQUIRED)

set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)

set(SOURCES
    src/main.cpp
    src/ScreenBuffer/ScreenBuffer.cpp
    src/App.hpp
    src/App.cpp
    src/Engine/Model/Model.hpp
    src/Engine/Model/Model.cpp
    src/Engine/Engine.hpp
    src/Engine/Engine.cpp
    src/Engine/Pipeline.hpp
    src/Engine/Pipeline.cpp
    src/Engine/Utility.hpp
    src/Engine/Shader.hpp
)

include_directories(third_party/stl_reader)

add_subdirectory(third_party/glfwext)
add_subdirectory(third_party/glewext)
add_subdirectory(third_party/tinyalgebra)
add_subdirectory(third_party/threadpool)

add_executable(${PROJECT_NAME} ${SOURCES})

add_compile_options(-ffast-math)
add_compile_options(-d)

target_link_libraries(${PROJECT_NAME}
    glfwext
    glewext
    tinyalgebra
    threadpool
)