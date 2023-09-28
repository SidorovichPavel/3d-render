#include <iostream>
#include <ranges>
#include <algorithm>
#include <list>

#include <stl_reader.h>

#include "Model.hpp"

Model::Model()
    : pool_(4),
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
    auto transform = projection * view * model_;

    auto ceil = vertices_.size() / 6;
    auto frac = vertices_.size() % 6;
    auto fnc = [](std::vector<ta::vec3>::iterator first, std::vector<ta::vec3>::iterator last, std::vector<ta::vec3>::iterator res_it, ta::mat4 trfm)
    {
        for (; first != last; ++first, ++res_it)
        {
            auto v4 = trfm * ta::vec4(*first, 1.f);
            auto rw = 1.f / v4.w();
            *res_it = ta::vec3(v4.x() * rw, v4.y() * rw, v4.z() * rw);
        }
    };

    std::list<std::future<void>> futures;
    for (int i = 0; i < 6; i++)
        futures.emplace_back(pool_.enqueue(fnc, vertices_.begin() + ceil * i, vertices_.begin() + ceil * (i + 1), result.begin() + ceil * i, transform));

    if (frac != 0)
        fnc(vertices_.begin() + ceil * 6, vertices_.end(), result.begin() + ceil * 6, transform);

    for (auto &&f : futures)
        f.get();

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
