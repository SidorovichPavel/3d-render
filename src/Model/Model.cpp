#include <iostream>
#include <ranges>
#include <algorithm>
#include <list>

#include <stl_reader.h>

#include "Model.hpp"

Model::Model()
    : pool_(4, [](std::vector<ta::vec3>::iterator first, std::vector<ta::vec3>::iterator last, std::vector<ta::vec3>::iterator res_it, ta::mat4& trfm)
        {
            for (; first != last; ++first, ++res_it)
            {
                auto v4 = trfm * ta::vec4(*first, 1.f);
                auto rw = 1.f / v4.w();

                *res_it = ta::vec3(v4.x() * rw, v4.y() * rw, v4.z() * rw);
            }
        }),
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

    return &vertices_[0][0];
}

unsigned int* Model::indices() noexcept
{
    if (triangles_.empty())
        return nullptr;

    return triangles_.data();
}

std::vector<ta::vec3> Model::transform(ta::mat4 view, ta::mat4 projection) noexcept
{
    std::vector<ta::vec3> result(vertices_.size());
    auto transform = projection * view * model_;

#ifdef NDEBUG

    auto ceil = vertices_.size() / block_count;
    auto frac = vertices_.size() % block_count;

    for (size_t i = 0; i < block_count; i++)
    {
        pool_.enqueue(vertices_.begin() + ceil * i, vertices_.begin() + ceil * (i + 1), result.begin() + ceil * i, transform);
    }

    if (frac != 0)
        pool_.enqueue(vertices_.begin() + ceil * block_count, vertices_.end(), result.begin() + ceil * block_count, transform);

    pool_.wait();

#else

    pool_.enqueue(vertices_.begin(), vertices_.end(), result.begin(), transform);
    pool_.wait();

#endif

    return result;
}

void Model::load_identity() noexcept
{
    model_ = ta::mat4(1.f);
}

void Model::rotare(const ta::vec3& axis, float angle)
{
    model_ = ta::rotate(model_, axis, angle);
}

void Model::translate(const ta::vec3& offset)
{
    model_ = ta::translate(model_, offset);
}
