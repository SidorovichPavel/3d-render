#pragma once

#include <cstdint>
#include <vector>
#include <utility>

namespace engine {

    class Pipeline final {
    public:
        Pipeline();
        ~Pipeline();

        // This method contain the logic for processing geometry, including vertex shader, clipping, and triangle setup.
        void ProcessGeometry();

        // This method contain the logic for rasterizing primitives.
        void Rasterize();

        // This method contain the logic for processing fragments and pixels.
        void FragmentShader();

        // Method for executing the entire pipeline
        void ExecutePipeline();

    private:

        std::pair<std::vector<uint32_t>::iterator, std::vector<uint32_t>::iterator> faces_range_;

    };

}