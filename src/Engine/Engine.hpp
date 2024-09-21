#pragma once

#include <memory>
#include <tuple>
#include <vector>

#include <glewext/glewext.hpp>
#include <glfwext/Window.hpp>
#include <threadpool/threadpool.hpp>
#include <tinyalgebra/math/type_decl.hpp>

#include "Model/Model.hpp"
#include "Pipeline.hpp"
#include "Shader.hpp"
#include "Utility.hpp"

namespace engine {

class Engine final {
 public:
  Engine(std::size_t tile_size);
  Engine(std::size_t width, std::size_t height, std::size_t tile_size);
  ~Engine();

  void init(std::size_t width, std::size_t height);
  void resize(std::size_t width, std::size_t height);
  void reset();

  void operator()(Model& model, IShader* shader, const ta::vec3& camera_pos);

  void display() const noexcept;

  void viewport(std::int32_t xmin, std::int32_t ymin, std::int32_t width,
                std::int32_t height) noexcept;

 private:
  std::size_t tile_size_;
  std::tuple<int, int> screen_size_;
  TileQueueGrid tile_queue_array_;
  mdspan<TileQueue*, 2> tile_queue_grid_;

  std::vector<float> zbuffer_;
  mdspan<float, 2> zgrid_;
  std::vector<float> screen_points_buffer_;
  // mdspan<float, 3> sp_grid_;
  std::vector<float> color_buffer_;
  mdspan<float, 3> colors_;

  ta::mat4 viewport_;
  GLuint VAO, VBOPos, VBOCol;
  std::unique_ptr<glewext::Shader> shader_;

  threadpool::threadpool pool_;
};

}  // namespace engine