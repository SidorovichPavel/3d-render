module;

#include <emmintrin.h>
#include <immintrin.h>
#include <xmmintrin.h>

#include <array>
#include <format>
#include <ranges>
#include <simd>
#include <string>

export module math;

export import :common;
export import math.exception;
export import math.types;

// namespace {
//
// using math::simd::mat4;
// using math::simd::mat4i;
//
// // https://godbolt.org/z/9fGjsG51W
//
// mat4 transpose2_impl(const mat4& mat) noexcept {
//   const auto vec = std::simd::partial_load<std::simd::vec<float>>(
//       std::span(mat), std::simd::flag_aligned);
//
//   return mat4(std::simd::permute(vec, [](const int i) {
//     const auto t = i * 4;
//     return t / 16 + t % 16;
//   }));
// }
//
// mat4i transpose2_impl(const mat4i& mat) noexcept {
//   const auto vec = std::simd::partial_load<std::simd::vec<int>>(
//       std::span(mat), std::simd::flag_aligned);
//
//   return mat4i(std::simd::permute(vec, [](const int i) {
//     const auto t = i * 4;
//     return t / 16 + t % 16;
//   }));
// }
//
// }  // namespace

export namespace math {

// https://godbolt.org/z/ffYKsz67c
// https://godbolt.org/z/re79Yas3e
//
// namespace simd {
//
// namespace transpose {
//
// mat4 transpose(const mat4& mat) noexcept {
//   mat4 result;
//
//   __m128 row1 = _mm_load_ps(mat[0].data());
//   __m128 row2 = _mm_load_ps(mat[1].data());
//   __m128 row3 = _mm_load_ps(mat[2].data());
//   __m128 row4 = _mm_load_ps(mat[3].data());
//
//   _MM_TRANSPOSE4_PS(row1, row2, row3, row4);
//
//   _mm_store_ps(result[0].data(), row1);
//   _mm_store_ps(result[1].data(), row2);
//   _mm_store_ps(result[2].data(), row3);
//   _mm_store_ps(result[3].data(), row4);
//
//   return result;
// }
//
// mat4 transpose2(const mat4& mat) noexcept { return ::transpose2_impl(mat); }
//
// mat4i transpose2(const mat4i& mat) noexcept { return ::transpose2_impl(mat);
// }
//
// }  // namespace transpose
//
// using transpose::transpose2;
//
// constexpr auto operator*(const mat4& mat,
//                          const VectorBatch& vec_batch) noexcept {
//   static_assert(std::ranges::distance(mat) == std::simd::vec<float>::size());
//
//   constexpr auto row_size = 4;
//
//   auto vmat = std::simd::unchecked_load<std::simd::vec<float>>(
//       std::span{mat.data(), std::simd::vec<float>::size()},
//       std::simd::flag_aligned);
//
//   auto data = std::simd::unchecked_load<std::simd::vec<float>>(
//       std::span{vec_batch.data(), std::simd::vec<float>::size()},
//       std::simd::flag_aligned);
//
//   for (const auto i : std::views::indices(row_size)) {
//     auto row = std::simd::permute(
//         vmat, [i, row_size](const int j) { return j % row_size; });
//
//     data += data * row;
//   }
//   return VectorBatch();
// }
//
// }  // namespace simd
//
// class VectorBatch {
//   static constexpr auto kSize = std::simd::vec<float>::size();
//
//  public:
//   struct Batch {
//     alignas(64) std::array<float, kSize> x;
//     alignas(64) std::array<float, kSize> y;
//     alignas(64) std::array<float, kSize> z;
//     alignas(64) std::array<float, kSize> w;
//   };
//
//   explicit VectorBatch() noexcept = default;
//
//   constexpr static auto Size() noexcept -> std::size_t { return kSize; }
//
//   explicit constexpr VectorBatch(const std::span<const vec4> data) {
//     if (data.size() > Size()) {
//       throw exception::UnavailableBatchData(
//           std::format("unavailable batch size: {}", data.size()));
//     }
//
//     auto tuples = std::views::zip(data_.x, data_.y, data_.z, data_.w);
//     auto [_, it] =
//         std::ranges::transform(data, tuples.begin(), [](const vec4& v) {
//           return std::make_tuple(v.x(), v.y(), v.z(), v.w());
//         });
//
//     std::ranges::fill(it, std::ranges::end(tuples),
//                       std::make_tuple(0.f, 0.f, 0.f, 0.f));
//   }
//
//   template <typename Self>
//   constexpr auto data(this Self&& self) noexcept {
//     return std::forward<Self>(self).data_.data();
//   }
//
//   template <typename Self>
//   constexpr auto operator[](this Self&& self, std::size_t i) noexcept {
//     return std::forward_as_tuple(std::forward<Self>(self).data_.x[i],
//                                  std::forward<Self>(self).data_.y[i],
//                                  std::forward<Self>(self).data_.z[i],
//                                  std::forward<Self>(self).data_.w[i]);
//   }
//
//  private:
//   alignas(64) Batch data_{};
// };
//
// static_assert(sizeof(VectorBatch) == 16 * sizeof(Vector<float, 4>));
// static_assert(sizeof(std::array<VectorBatch, 4>) ==
//               4 * 16 * sizeof(Vector<float, 4>));
//
// std::vector<VectorBatch> ToBatches(const std::span<const vec4> data) noexcept
// {
//   auto result = std::vector<VectorBatch>();
//
//   const auto batch_number = data.size() / VectorBatch::Size();
//
//   for (const auto batch_idx : indices(batch_number)) {
//     auto subspan =
//         data.subspan(VectorBatch::Size() * batch_idx, VectorBatch::Size());
//     result.emplace_back(subspan);
//   }
//
//   const auto additional_batch_size = data.size() % VectorBatch::Size();
//   if (additional_batch_size > 0u) {
//     auto subspan =
//         data.subspan(VectorBatch::Size() * batch_number,
//         additional_batch_size);
//     result.emplace_back(subspan);
//   }
//
//   return result;
// }

template <typename T, std::size_t N, std::size_t M, std::size_t Alignment>
constexpr auto transpose(const Matrix<T, M, N, Alignment>& mat) noexcept {
  using std::views::indices;

  Matrix<T, N, M, Alignment> result;
  for (const auto i : indices(M)) {
    for (const auto j : indices(M)) {
      result[j, i] = mat[i, j];
    }
  }

  return result;
}

template <typename T, std::size_t M, std::size_t K, std::size_t N>
auto operator*(const Matrix<T, M, K>& m1, const Matrix<T, K, N>& m2) noexcept {
  using std::views::indices;

  Matrix<T, M, N> result;

  for (auto i : indices(M)) {
    for (auto k : indices(K)) {
      for (auto j : indices(N)) {
        result[i, j] += m1[i, k] * m2[k, j];
      }
    }
  }

  return result;
}

template <typename T, std::size_t N, std::size_t M, std::size_t Alignment>
constexpr auto operator*(const Matrix<T, M, N, Alignment>& mat,
                         const Vector<T, N, Alignment>& vec) noexcept {
  using std::views::indices;

  Vector<T, M, Alignment> result;

  for (const auto i : indices(M)) {
    for (const auto j : indices(M)) {
      result[i] += mat[i, j] * vec[j];
    }
  }

  return result;
}

}  // namespace math

export namespace std {

template <typename CharT, typename T, std::size_t M, std::size_t N,
          std::size_t Alignment>
struct formatter<math::Matrix<T, M, N, Alignment>, CharT> {
  template <typename FormatParseContext>
  constexpr auto parse(FormatParseContext& pc) {
    // parse formatter args like padding, precision if you support it
    return pc.begin();  // empty format specs
  }

  template <typename FormatContext>
  auto format(const math::Matrix<T, M, N, Alignment>& m,
              FormatContext& ctx) const {
    std::string result("[\n");

    for (bool first = true; auto chunk : m | std::views::chunk(N)) {
      if (first) {
        first = false;
      } else {
        result += ",\n";
      }

      result += std::format("\t{}", chunk);
    }

    result += "\n]";

    return std::format_to(ctx.out(), "{}", result);
  }
};

}  // namespace std
