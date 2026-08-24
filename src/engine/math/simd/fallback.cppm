module;

#include <memory>

export module math.simd.fallback;

import math.simd.common;
import math.exception;

namespace math::simd::fallback {

namespace {

class FallbackExecutor final : public common::ISimdExecutor {
 public:
  FallbackExecutor() = default;
  ~FallbackExecutor() override = default;

  mat2 mul(const mat2& a, const mat2& b) const override {
    throw exception::SimdNotImplemented("mat2*mat2");
  }

  mat2i mul(const mat2i& a, const mat2i& b) const override {
    throw exception::SimdNotImplemented("mat2i*mat2i");
  }

  mat3 mul(const mat3& a, const mat3& b) const override {
    throw exception::SimdNotImplemented("mat3*mat3");
  }

  mat3i mul(const mat3i& a, const mat3i& b) const override {
    throw exception::SimdNotImplemented("mat3i*mat3i");
  }

  mat4 mul(const mat4&, const mat4&) const override {
    throw exception::SimdNotImplemented("mat4*mat4");
  }

  mat4i mul(const mat4i&, const mat4i&) const override {
    throw exception::SimdNotImplemented("mat4i*mat4i");
  }
};

}  // namespace

export auto MakeExecutor() noexcept -> std::unique_ptr<common::ISimdExecutor> {
  return std::make_unique<FallbackExecutor>();
}

}  // namespace math::simd::fallback
