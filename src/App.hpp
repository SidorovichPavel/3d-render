#pragma once
#include <GL/glew.h>

#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <format>
#include <iostream>
#include <memory>
#include <numeric>
#include <ranges>
#include <string>
#include <string_view>
#include <vector>

#include <glewext/Shader.hpp>
#include <glewext/Texture.hpp>
#include <glewext/glewext.hpp>
#include <glfwext/Window.hpp>
#include <glfwext/glfwext.hpp>
#include <threadpool/threadpool.hpp>
#include <tinyalgebra/Camera.hpp>
#include <tinyalgebra/math/math.hpp>

#include "Engine/Model/Model.hpp"
#include "ScreenBuffer/ScreenBuffer.hpp"

#include "Engine/Engine.hpp"

class App {
 public:
  App(int argc, char* args[]);
  ~App();

  void run();

 private:
  void movement(float t) noexcept;

  threadpool::threadpool pool;

  std::unique_ptr<glfwext::Window> window;

  bool mouse_input{false};
  float lastX, lastY;
  float fovy{90.f};

  engine::Model model;
  ta::Camera camera;

  engine::Engine engine_;
};