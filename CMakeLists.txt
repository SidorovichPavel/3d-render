cmake_minimum_required(VERSION 3.5.0)
project(3d-render VERSION 0.1.0 LANGUAGES C CXX)

find_package(GLEW REQUIRED)

set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/Model/Model.cpp
)

set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 23 CXX_STANDARD_REQUIRED On)

include_directories(stl_reader)

add_subdirectory(glfwextlib)
add_subdirectory(tinyalgebralib)
add_subdirectory(threadpoollib)

target_link_libraries(${PROJECT_NAME} 
    glfwextlib
    GLEW
    OpenGL
    tinyalgebralib
    threadpoollib
    )