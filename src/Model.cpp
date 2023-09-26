#include <iostream>
#include <stl_reader.h>

#include "Model.hpp"

Model::Model()
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
