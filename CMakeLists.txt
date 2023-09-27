cmake_minimum_required(VERSION 3.5.0)
project(3d-render VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -std=gnu++23)

find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)

set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)

add_executable(3d-render
    src/main.cpp
    src/Model.cpp
    src/Window/window.cpp
)

include_directories(stl_reader)

add_subdirectory(tinyalgebralib)
add_subdirectory(threadpoollib)

target_link_libraries(${PROJECT_NAME} 
    GLEW
    glfw
    OpenGL
    tinyalgebralib
    threadpoollib
    )

set(CMAKE_CXX_STANDARD 23)