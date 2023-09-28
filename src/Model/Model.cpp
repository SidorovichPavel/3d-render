#include <iostream>
#include <ranges>
#include <algorithm>

#include <stl_reader.h>

#include "Model.hpp"

Model::Model()
    : model_(1.f)
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
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }

    const size_t numTris = triangles_.size() / 3;
    for (size_t itri = 0; itri < numTris; ++itri)
    {
        for (size_t icorner = 0; icorner < 3; ++icorner)
        {
            float *c = &coords[3 * triangles_[3 * itri + icorner]];
            vertices_.emplace_back(c[0], c[1], c[2]);

            float *n = &normals[3 * triangles_[3 * itri + icorner]];
            normals_.emplace_back(n[0], n[1], n[2]);
        }
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

float *Model::vdata() noexcept
{
    if (vertices_.empty())
        return nullptr;

    return &vertices_[0][0];
}

unsigned int *Model::indices() noexcept
{
    if (triangles_.empty())
        return nullptr;

    return triangles_.data();
}

std::vector<ta::vec3> Model::transform(ta::mat4 view, ta::mat4 projection) noexcept
{
    std::vector<ta::vec3> result(vertices_.size());

    std::ranges::transform(vertices_, result.begin(), [&](ta::vec3 &vec) {
        auto v4 = projection * view * model_ * ta::vec4(vec, 1.f);
        auto rw = 1.f / v4.w();
        return ta::vec3(v4.x() * rw, v4.y() * rw, v4.z() * rw);
    });

    return result;
}

void Model::load_identity() noexcept
{
    model_ = ta::mat4(1.f);
}

void Model::rotare(const ta::vec3 &axis, float angle)
{
    model_ = ta::rotate(model_, axis, angle);
}
