#pragma once

#include <vector>
#include <string_view>

#include <tinyalgebralib/math/math.hpp>
#include <threadpoollib/threadpool.hpp>

class Model
{
public:

    Model();
    ~Model();

    void load_from_file(std::string_view file);
    size_t vertex_count() const noexcept;

    size_t indices_count() const noexcept;
    size_t indices_sizeof() const noexcept;

    float* vdata() noexcept;
    unsigned int* indices() noexcept;

    /* return vertex data and indices data */
    std::tuple<std::vector<ta::vec3>, std::vector<uint32_t>> transform(ta::mat4 view, ta::mat4 projection) noexcept;
    /* return vertex data and indices data */
    std::tuple<std::vector<ta::vec3>, std::vector<uint32_t>> transform(ta::mat4 view, ta::mat4 projection, threadpool::threadpool& pool) noexcept;

    void load_identity() noexcept;
    void scale(const ta::vec3& size);
    void rotare(const ta::vec3& axis, float angle);
    void translate(const ta::vec3& offset);

private:
    std::vector<ta::vec3> vertices_;
    std::vector<ta::vec3> normals_;
    std::vector<uint32_t> triangles_;
    std::vector<uint32_t> solid_ranges_;

    static constexpr size_t chunk_count = 8;
    ta::mat4 model_;
};