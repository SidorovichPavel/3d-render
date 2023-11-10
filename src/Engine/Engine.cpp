#include "Engine.hpp"

#include <limits>
#include <numeric>
#include <stdexcept>

namespace engine
{
    // Engine::Engine(std::size_t tile_size)
    //     :
    //     tile_size_(tile_size) {

    //     throw std::runtime_error("dont have impl");
    // }

    Engine::Engine(std::size_t width, std::size_t height, std::size_t tile_size)
        :
        tile_size_(tile_size),
        zbuffer_(width* height, std::numeric_limits<float>::max()),
        screen_points_buffer_(width* height * 2),
        color_buffer_(width* height * 3, .7f) {

        zgrid_ = mdspan<float, 2>(zbuffer_.data(), height, width);

        auto sp_grid_ = mdspan<float, 3>(screen_points_buffer_.data(), height, width, std::size_t(2));
        for (std::size_t i = 0;i < height;i++)
            for (std::size_t j = 0;j < width;j++) {
                sp_grid_[i][j][0] = 2.f * j / width - 1.f;
                sp_grid_[i][j][1] = 2.f * i / height - 1.f;
            }

        colors_ = mdspan<float, 3>(color_buffer_.data(), height, width, static_cast<std::size_t>(3));

        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        glGenBuffers(1, &VBOPos);
        glBindBuffer(GL_ARRAY_BUFFER, VBOPos);
        glBufferData(GL_ARRAY_BUFFER, screen_points_buffer_.size() * sizeof(float), screen_points_buffer_.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_TRUE, 0, nullptr);
        glEnableVertexAttribArray(0);

        glGenBuffers(1, &VBOCol);
        glBindBuffer(GL_ARRAY_BUFFER, VBOCol);
        glBufferData(GL_VERTEX_ARRAY, color_buffer_.size() * sizeof(float), color_buffer_.data(), GL_STREAM_DRAW);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 0, nullptr);
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);

        std::string_view vshader("/mnt/sata0/Workshop/3d-render/resources/glsl/main.vert");
        std::string_view fshader("/mnt/sata0/Workshop/3d-render/resources/glsl/main.frag");

        shader_ = std::make_unique<glewext::Shader>(vshader, fshader);
    }

    Engine::~Engine() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBOPos);
        glDeleteBuffers(1, &VBOCol);
    }

    void Engine::reset() {
        for (auto&& q : tile_queue_array_)
            q = TileQueue();

        for (auto&& z : zbuffer_)
            z = std::numeric_limits<float>::max();

        // sequential color component
        for (auto&& scc : color_buffer_)
            scc = 0.f;
    }

    void Engine::operator()(Model& model) {

    }

    void Engine::display() const noexcept {
        shader_->use();
        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBOCol);
        glBufferData(GL_ARRAY_BUFFER, color_buffer_.size() * sizeof(float), color_buffer_.data(), GL_STREAM_DRAW);

        glDrawArrays(GL_POINTS, 0, screen_points_buffer_.size() / 2);
        glBindVertexArray(0);
    }

}