#include "Engine.hpp"

#include <limits>
#include <numeric>
#include <stdexcept>
#include <map>
#include <array>

#include <iostream>
#include <format>

#include <tinyalgebralib/math/math.hpp>

namespace engine
{
    Engine::Engine(std::size_t tile_size)
        :
        tile_size_(tile_size),
        pool_(10)
    {

    }

    Engine::Engine(std::size_t width, std::size_t height, std::size_t tile_size)
        :
        tile_size_(tile_size),
        screen_size_(static_cast<int>(width), static_cast<int>(height)),
        zbuffer_(width* height, std::numeric_limits<float>::max()),
        screen_points_buffer_(width* height * 2),
        color_buffer_(width* height * 3, .7f),
        pool_(10)
    {

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

    void Engine::init(std::size_t width, std::size_t height) {

        screen_size_ = std::make_tuple(static_cast<int>(width), static_cast<int>(height));
        zbuffer_ = std::vector<float>(width * height, std::numeric_limits<float>::max());
        screen_points_buffer_ = std::vector<float>(width * height * 2);
        color_buffer_ = std::vector<float>(width * height * 3, 0.7f);

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

    void Engine::resize(std::size_t width, std::size_t height) {

        zbuffer_ = std::vector<float>(width * height, std::numeric_limits<float>::max());
        screen_points_buffer_ = std::vector<float>(width * height * 2);
        color_buffer_ = std::vector<float>(width * height * 3, 0.7f);

        zgrid_ = mdspan<float, 2>(zbuffer_.data(), height, width);

        auto sp_grid_ = mdspan<float, 3>(screen_points_buffer_.data(), height, width, std::size_t(2));
        for (std::size_t i = 0;i < height;i++)
            for (std::size_t j = 0;j < width;j++) {
                sp_grid_[i][j][0] = 2.f * j / width - 1.f;
                sp_grid_[i][j][1] = 2.f * i / height - 1.f;
            }

        colors_ = mdspan<float, 3>(color_buffer_.data(), height, width, static_cast<std::size_t>(3));

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBOPos);
        glBufferData(GL_ARRAY_BUFFER, screen_points_buffer_.size() * sizeof(float), screen_points_buffer_.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, VBOCol);
        glBufferData(GL_VERTEX_ARRAY, color_buffer_.size() * sizeof(float), color_buffer_.data(), GL_STREAM_DRAW);

        glBindVertexArray(0);
    }

    void Engine::reset() {
        for (auto&& q : tile_queue_array_)
            q = TileQueue();

        for (auto&& z : zbuffer_)
            z = std::numeric_limits<float>::max();

        // sequential color component
        for (auto&& scc : color_buffer_)
            scc = .3f;
    }

    bool is_outside(float a0, float b0, float a1, float b1, float a2, float b2) {
        bool f1 = (a0 < b0);
        bool f2 = (a1 < b1);
        bool f3 = (a2 < b2);

        return f1 && f2 && f3;
    };

    bool is_inside(float a0, float b0, float a1, float b1, float a2, float b2) {
        bool f1 = (a0 <= b0);
        bool f2 = (a1 <= b1);
        bool f3 = (a2 <= b2);

        return f1 && f2 && f3;
    }

    void Engine::operator()(Model& model, IShader* shader, const ta::vec3& camera_pos) {

        std::map<std::size_t, ta::vec4> cache;

        ta::vec3 light_pos(0.f, 100.f, 0.f);
        //ta::vec3 light_color(1.f);
        //ta::vec3 base_color(0.9f, 0.f, 0.f);

        decltype(auto) mesh = model.mesh();

        for (std::size_t solid_idx = 0; solid_idx != mesh.num_solids(); solid_idx++) {
            for (auto tri_idx = mesh.solid_tris_begin(solid_idx);
                tri_idx != mesh.solid_tris_end(solid_idx);
                tri_idx++) {

                std::array<ta::vec4, 3> vtcs;
                auto a3f = mesh.tri_normal(tri_idx);
                ta::vec3 normal(a3f[0], a3f[1], a3f[2]);

                for (auto&& [idx, vclip] : vtcs | std::views::enumerate) {
                    auto vidx = mesh.tri_corner_ind(tri_idx, idx);
                    auto finder = cache.find(vidx);
                    if (finder == cache.end()) {
                        auto a3f = mesh.vrt_coords(vidx);
                        ta::vec3 v(a3f[0], a3f[1], a3f[2]);
                        vclip = shader->Vertex(v);
                        cache[vidx] = vclip;
                    }
                    else {
                        vclip = cache[vidx];
                    }
                }

                auto& v0 = vtcs[0], & v1 = vtcs[1], & v2 = vtcs[2];

                // x < -w
                bool outside_left = is_outside(v0.x(), -v0.w(), v1.x(), -v1.w(), v2.x(), -v2.w());
                // x > w
                bool outside_right = is_outside(v0.w(), v0.x(), v1.w(), v1.x(), v2.w(), v2.x());
                // y < -w
                bool outside_bottom = is_outside(v0.y(), -v0.w(), v1.y(), -v1.w(), v2.y(), -v2.w());
                // y > w
                bool outside_top = is_outside(v0.w(), v0.y(), v1.w(), v1.y(), v2.w(), v2.y());
                // z < -w
                bool outside_near = is_outside(v0.z(), -v0.w(), v1.z(), -v1.w(), v2.z(), -v2.w());
                // z > w
                bool outside_far = is_outside(v0.w(), v0.z(), v1.w(), v1.z(), v2.w(), v2.z());

                auto f1 = outside_left || outside_right;
                auto f2 = outside_bottom || outside_top;
                auto f3 = outside_near || outside_far;
                auto f4 = f1 || f2 || f3;

                if (f4)
                    // trivial reject
                    continue;
                else {
                    v0 /= v0.w();
                    v1 /= v1.w();
                    v2 /= v2.w();

                    if (v0.y() < -1.f + std::numeric_limits<float>::epsilon())
                        std::cout << std::format("y in v0 outside fructum: {}", v0.y()) << std::endl;

                    if (v1.y() < -1.f + std::numeric_limits<float>::epsilon())
                        std::cout << std::format("y in v1 outside fructum: {}", v1.y()) << std::endl;

                    if (v2.y() < -1.f + std::numeric_limits<float>::epsilon())
                        std::cout << std::format("y in v2 outside fructum: {}", v2.y()) << std::endl;

                    std::array<ta::vec2i, 3> vp_vtcs;
                    auto tmp = viewport_ * v0;
                    vp_vtcs[0] = ta::vec2i(static_cast<std::int32_t>(tmp.x()), static_cast<std::int32_t>(tmp.y()));
                    tmp = viewport_ * v1;
                    vp_vtcs[1] = ta::vec2i(static_cast<std::int32_t>(tmp.x()), static_cast<std::int32_t>(tmp.y()));
                    tmp = viewport_ * v2;
                    vp_vtcs[2] = ta::vec2i(static_cast<std::int32_t>(tmp.x()), static_cast<std::int32_t>(tmp.y()));

                    auto&& [width, height] = screen_size_;
                    ta::vec2i bboxmin(width - 1, height - 1);
                    ta::vec2i bboxmax(0, 0);
                    ta::vec2i clamp = bboxmin;

                    for (auto&& v : vp_vtcs)
                    {
                        bboxmin.x() = std::clamp(static_cast<int>(std::ceil(v.x())), 0, bboxmin.x());
                        bboxmin.y() = std::clamp(static_cast<int>(std::ceil(v.y())), 0, bboxmin.y());

                        bboxmax.x() = std::clamp(static_cast<int>(std::ceil(v.x())), bboxmax.x(), clamp.x());
                        bboxmax.y() = std::clamp(static_cast<int>(std::ceil(v.y())), bboxmax.y(), clamp.y());
                    }

                    ta::vec2i p;

                    for (p.x() = bboxmin.x(); p.x() <= bboxmax.x(); p.x()++)
                    {
                        for (p.y() = bboxmin.y(); p.y() <= bboxmax.y(); p.y()++)
                        {
                            auto b = ta::barycentric(vp_vtcs[0], vp_vtcs[1], vp_vtcs[2], p);
                            if (!b || (b->x() < 0.f) || (b->y() < 0.f) || (b->z() < 0.f))
                                continue;

                            float z = 0.f;
                            for (auto&& [v, bc] : std::views::zip(vtcs, *b))
                                z += v.z() * bc;

                            if (z < zgrid_[p.y()][p.x()])
                            {
                                zgrid_[p.y()][p.x()] = z;

                                ta::vec4 tipos(0.f);
                                for (auto&& [p, bc] : std::views::zip(vtcs, *b)) {
                                    tipos += p * bc;
                                }

                                ta::vec3 ipos(tipos);
                                auto light_dir = ta::normalize(ipos - light_pos);
                                auto dir = ta::normalize(ipos - camera_pos);

                                auto cos = ta::dot(-light_dir, normal);
                                cos = std::clamp(cos, 0.f, 1.f);

                                colors_[p.y()][p.x()][0] = cos;
                                colors_[p.y()][p.x()][1] = cos;
                                colors_[p.y()][p.x()][2] = cos;
                            }
                        }
                    }
                }

            }
        }
    }

    void Engine::display() const noexcept {

        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.2f, 0.3f, 0.3f, 1.f);

        shader_->use();
        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBOCol);
        glBufferData(GL_ARRAY_BUFFER, color_buffer_.size() * sizeof(float), color_buffer_.data(), GL_STREAM_DRAW);

        glDrawArrays(GL_POINTS, 0, screen_points_buffer_.size() / 2);
        glBindVertexArray(0);
    }

    void Engine::viewport(std::int32_t xmin, std::int32_t ymin, std::int32_t width, std::int32_t height) noexcept {
        viewport_ = ta::viewport(xmin, ymin, width, height);
    }

}