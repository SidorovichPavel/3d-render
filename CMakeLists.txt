cmake_minimum_required(VERSION 3.25.0)
project(3d-render VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(GLEW REQUIRED)

set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)

set(SOURCES
    src/main.cpp
    src/Model/Model.cpp
    src/ScreenBuffer.cpp
)

include_directories(stl_reader)

add_subdirectory(glfwextlib)
add_subdirectory(glewextlib)
add_subdirectory(tinyalgebralib)
add_subdirectory(threadpoollib)

add_executable(${PROJECT_NAME} ${SOURCES})

add_compile_options(-march=native)
add_compile_options(-d)

target_link_libraries(${PROJECT_NAME} 
    glfwextlib
    glewextlib
    tinyalgebralib
    threadpoollib
)