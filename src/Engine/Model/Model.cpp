#include <iostream>
#include <ranges>
#include <algorithm>
#include <list>

#include "Model.hpp"

namespace engine {

    Model::Model()
        :
        model_(1.f)
    {
    }

    Model::~Model()
    {
    }

    void Model::load_from_file(std::string_view strv) {
        stl_mesh_.read_file(strv.data());
    }

    const stl_reader::StlMesh<float, std::size_t>& Model::mesh() {
        return stl_mesh_;
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

    ta::mat4 Model::mat4() const noexcept {
        return model_;
    }
}