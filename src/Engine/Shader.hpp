#pragma once

#include <tinyalgebralib/math/math.hpp>

namespace engine {

    class IShader
    {
    public:

        IShader() = default;
        virtual ~IShader() = default;

        virtual ta::vec4 Vertex(ta::vec3 pos) = 0;
        virtual ta::vec4 Fragment() = 0;
    };


}