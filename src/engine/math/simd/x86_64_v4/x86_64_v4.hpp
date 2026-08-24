#pragma once

// work as is! don't touch!!!

import math.simd.common;

namespace math::simd::x86_64_v4::impl {

auto mul(const mat4& a, const mat4& b) noexcept -> mat4;

auto mul(const mat4i& a, const mat4i& b) noexcept -> mat4i;

}
