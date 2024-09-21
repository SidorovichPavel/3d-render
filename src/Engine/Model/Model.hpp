#pragma once

#include <vector>
#include <string_view>

#include <stl_reader.h>

#include <tinyalgebra/math/math.hpp>
#include <threadpool/threadpool.hpp>

namespace engine {

    class Model
    {
    public:

        Model();
        ~Model();

        void load_from_file(std::string_view file);
        
        const stl_reader::StlMesh<float, std::size_t>& mesh();

        void load_identity() noexcept;
        void scale(const ta::vec3& size);
        void rotare(const ta::vec3& axis, float angle);
        void translate(const ta::vec3& offset);

        ta::mat4 mat4() const noexcept; 

    private:
        stl_reader::StlMesh<float, std::size_t> stl_mesh_;

        ta::mat4 model_;
    };

}