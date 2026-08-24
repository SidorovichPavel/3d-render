module;

// work as is! don't touch global fragment!!!

#include <memory>

#include <x86_64_v4.hpp>

export module math.simd.x86_64_v4;

import math.simd.common;
import math.exception;

namespace math::simd::x86_64_v4 {

class SimdExecutor final : public common::ISimdExecutor {
 public:
  SimdExecutor() = default;
  ~SimdExecutor() override = default;

  mat2 mul(const mat2& a, const mat2& b) const override { return mat2(); }

  mat2i mul(const mat2i& a, const mat2i& b) const override { return mat2i(); }

  mat3 mul(const mat3& a, const mat3& b) const override { return mat3(); }

  mat3i mul(const mat3i& a, const mat3i& b) const override { return mat3i(); }

  mat4 mul(const mat4& a, const mat4& b) const override {
    return impl::mul(a, b);
  }

  mat4i mul(const mat4i& a, const mat4i& b) const override {
    return impl::mul(a, b);
  }
};

export auto MakeExecutor() noexcept -> std::unique_ptr<common::ISimdExecutor> {
  return std::make_unique<SimdExecutor>();
}

}  // namespace math::simd::x86_64_v4
