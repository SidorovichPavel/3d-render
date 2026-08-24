#include <ranges>
#include <simd>

// work as is! don't touch!!!

#include <x86_64_v2.hpp>

namespace math::simd::x86_64_v2::impl {
namespace {

template <int I>
using ic = std::integral_constant<int, I>;

using std::simd::permute;
using std::simd::vec;

template <typename T>
constexpr auto mul_impl([[maybe_unused]] const vec<T> va,
                        [[maybe_unused]] const vec<T> vb) noexcept {
  using std::simd::permute;

  return vec<T>{};
}

// template <std::ranges::contiguous_range Rn>
// constexpr auto unchecked_load_align(Rn&& rn) noexcept
//     -> vec<std::ranges::range_value_t<Rn>> {
//   using range_t = std::ranges::range_value_t<Rn>;
//   static_assert(std::ranges::distance(rn) == vec<range_t>::size());
//   return unchecked_load(rn.cbegin(), rn.cend(), std::simd::flag_aligned);
// }

}  // namespace

auto mul(const mat4& a, const mat4& b) noexcept -> mat4 { return {}; }

auto mul(const mat4i& a, const mat4i& b) noexcept -> mat4i { return {}; }

}  // namespace math::simd::x86_64_v2::impl
