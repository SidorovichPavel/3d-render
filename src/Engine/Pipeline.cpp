#include "Pipeline.hpp"

namespace engine {

Pipeline::Pipeline() {}

Pipeline::~Pipeline() {}

void Pipeline::ProcessGeometry() {}

void Pipeline::Rasterize() {}

void Pipeline::FragmentShader() {}

void Pipeline::ExecutePipeline() {
  ProcessGeometry();
  Rasterize();
  FragmentShader();
}

}  // namespace engine