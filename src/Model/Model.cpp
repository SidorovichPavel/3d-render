#include <iostream>
#include <ranges>
#include <algorithm>
#include <list>

#include <stl_reader.h>

#include "Model.hpp"

Model::Model()
    :
    model_(1.f)
{
}

Model::~Model()
{
}

void Model::load_from_file(std::string_view strv)
{
    std::vector<float> coords, normals;
    try
    {
        stl_reader::ReadStlFile(strv.data(), coords, normals, triangles_, solid_ranges_);
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    for (auto vit = coords.begin(),
        nit = normals.begin();
        vit != coords.end();)
    {
        vertices_.emplace_back(*vit, *(vit + 1), *(vit + 2));
        normals_.emplace_back(*nit, *(nit + 1), *(nit + 2));

        vit += 3;
        nit += 3;
    }
}

size_t Model::vertex_count() const noexcept
{
    return vertices_.size() * 3;
}

size_t Model::indices_count() const noexcept
{
    return triangles_.size();
}

size_t Model::indices_sizeof() const noexcept
{
    return triangles_.size() * sizeof(unsigned int);
}

float* Model::vdata() noexcept
{
    if (vertices_.empty())
        return nullptr;

    return vertices_.data()->data();
}

unsigned int* Model::indices() noexcept
{
    if (triangles_.empty())
        return nullptr;

    return triangles_.data();
}

std::tuple<std::vector<ta::vec3>, std::vector<uint32_t>> Model::transform(ta::mat4 view, ta::mat4 projection) noexcept
{
    std::vector<ta::vec3> result(vertices_.size());
    auto transform = projection * view * model_;

    std::ranges::transform(vertices_, result.begin(), [&](const ta::vec3& vec) {
        auto v4 = transform * ta::vec4(vec, 1.f);
        v4 /= v4.w();

        return ta::vec3(v4);
        });

    return std::make_tuple(result, triangles_);
}

std::tuple<std::vector<ta::vec3>, std::vector<uint32_t>> Model::transform(ta::mat4 view, ta::mat4 projection, threadpool::threadpool& pool) noexcept
{
    std::vector<ta::vec3> result(vertices_.size());
    auto transform = projection * view * model_;

    auto chunk_size = vertices_.size() / chunk_count;

    pool.transform(vertices_, result.begin(), [transform](const ta::vec3& vec) {
        auto v4 = transform * ta::vec4(vec, 1.f);
        v4 /= v4.w();

        return ta::vec3(v4);
        });

    return std::make_tuple(result, triangles_);
}

void Model::load_identity() noexcept
{
    model_ = ta::mat4(1.f);
}

void Model::scale(const ta::vec3& size)
{
    model_ = ta::scale(model_, size);
}

void Model::rotare(const ta::vec3& axis, float angle)
{
    model_ = ta::rotate(model_, axis, angle);
}

void Model::translate(const ta::vec3& offset)
{
    model_ = ta::translate(model_, offset);
}
