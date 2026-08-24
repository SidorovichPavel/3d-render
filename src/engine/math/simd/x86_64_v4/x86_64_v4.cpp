#include <iterator>
#include <ranges>
#include <simd>

// work as is! don't touch!!!

#include <x86_64_v4.hpp>

namespace math::simd::x86_64_v4::impl {
namespace {

template <int I>
using ic = std::integral_constant<int, I>;

using std::simd::permute;
using std::simd::vec;

template <typename T>
constexpr auto mul_impl(const vec<T> va, const vec<T> vb) noexcept -> vec<T> {
  const auto f = [&]<int k>(ic<k>) noexcept {
    const auto pa = permute(va, [](const int i) { return i % 4 + k * 4; });
    const auto pb = permute(vb, [](const int i) { return (i / 4) * 4 + k; });
    return pa * pb;
  };

  return [&]<int... I>(std::integer_sequence<int, I...>) noexcept {
    return (f(ic<I>{}) + ...);
  }(std::make_integer_sequence<int, 4>());
}

// template <typename T>
// constexpr auto mul_impl2(const vec<T> va, const vec<T> vb) noexcept -> vec<T>
// {
//   constexpr std::array index_table{
//       std::array{0, 0, 0, 0, 5, 5, 5, 5, 10, 10, 10, 10, 15, 15, 15, 15},
//       std::array{1, 1, 1, 1, 6, 6, 6, 6, 11, 11, 11, 11, 12, 12, 12, 12},
//       std::array{2, 2, 2, 2, 7, 7, 7, 7, 8, 8, 8, 8, 13, 13, 13, 13},
//       std::array{3, 3, 3, 3, 4, 4, 4, 4, 9, 9, 9, 9, 14, 14, 14, 14}};
//
//   const auto f = [&]<int K>(ic<K>) noexcept -> vec<T> {
//     const auto vat = permute(va, [](const int i) { return (i + K * 4) % 16;
//     }); const auto vbt = permute(vb, [&](const int i) {
//       constexpr auto ids = index_table[K];
//       return ids[i];
//     });
//     return vat * vbt;
//   };
//
//   return [&]<int... Ks>(std::integer_sequence<int, Ks...>) noexcept -> vec<T>
//   {
//     return (f(ic<Ks>{}) + ...);
//   }(std::make_integer_sequence<int, 4>());
// }

template <std::ranges::contiguous_range Rn>
constexpr auto unchecked_load_align(Rn&& rn) noexcept
    -> vec<std::ranges::range_value_t<Rn>> {
  using range_t = std::ranges::range_value_t<Rn>;
  static_assert(std::ranges::distance(rn) == vec<range_t>::size());
  return unchecked_load(rn.cbegin(), rn.cend(), std::simd::flag_aligned);
}

}  // namespace

auto mul(const mat4& a, const mat4& b) noexcept -> mat4 {
  using std::simd::unchecked_store;

  const auto va = unchecked_load_align(a);
  const auto vb = unchecked_load_align(b);

  const auto vr = mul_impl(va, vb);

  mat4 res;
  unchecked_store(vr, res, std::simd::flag_aligned);

  return res;
}

auto mul(const mat4i& a, const mat4i& b) noexcept -> mat4i {
  using std::simd::unchecked_store;

  const auto va = unchecked_load_align(a);
  const auto vb = unchecked_load_align(b);

  const auto vr = mul_impl(va, vb);

  mat4i res;
  unchecked_store(vr, res, std::simd::flag_aligned);

  return res;
}

}  // namespace math::simd::x86_64_v4::impl
