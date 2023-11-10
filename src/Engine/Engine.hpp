#pragma once

#include <vector>
#include <tuple>
#include <memory>

#include <threadpoollib/threadpool.hpp>
#include <glewextlib/glewext.hpp>
#include <glfwextlib/Window.hpp>

#include "Pipeline.hpp"
#include "Model/Model.hpp"
#include "Utility.hpp"

namespace engine {

    class Engine final
    {
    public:

        //Engine(std::size_t tile_size);
        Engine(std::size_t width, std::size_t height, std::size_t tile_size);
        ~Engine();

        //void init(std::size_t width, std::size_t height);
        void reset();

        void operator()(Model& model);

        void display() const noexcept;

    private:
        std::size_t tile_size_;
        TileQueueGrid tile_queue_array_;
        mdspan<TileQueue*, 2> tile_queue_grid_;

        std::vector<float> zbuffer_;
        mdspan<float, 2> zgrid_;
        std::vector<float> screen_points_buffer_;
        //mdspan<float, 3> sp_grid_;
        std::vector<float> color_buffer_;
        mdspan<float, 3> colors_;

        GLuint VAO, VBOPos, VBOCol;
        std::unique_ptr<glewext::Shader> shader_;
    };

}