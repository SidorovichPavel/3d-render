#pragma once
#include <GL/glew.h>

#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include <ranges>
#include <algorithm>
#include <memory>
#include <chrono>
#include <numeric>
#include <array>
#include <cmath>
#include <format>

#include <threadpoollib/threadpool.hpp>
#include <tinyalgebralib/math/math.hpp>
#include <tinyalgebralib/Camera.hpp>
#include <glfwextlib/glfwext.hpp>
#include <glfwextlib/Window.hpp>
#include <glewextlib/glewext.hpp>
#include <glewextlib/Shader.hpp>
#include <glewextlib/Texture.hpp>

#include "Engine/Model/Model.hpp"
#include "ScreenBuffer/ScreenBuffer.hpp"

#include "Engine/Engine.hpp"

class App
{
public:
    App(int argc, char* args[]);
    ~App();

    void run();

private:

    void movement(float t) noexcept;

    threadpool::threadpool pool;

    std::unique_ptr<glfwext::Window> window;

    bool mouse_input{ false };
    float lastX, lastY;
    float fovy{ 90.f };

    engine::Model model;
    ta::Camera camera;

    std::unique_ptr<engine::Engine> engine_;
};