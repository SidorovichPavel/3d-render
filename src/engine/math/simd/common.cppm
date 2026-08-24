module;

#include <cstddef>
#include <ranges>

export module math.simd.common;

export import math.types;

export namespace math::simd {

constexpr std::size_t kDefaultMatrixAlignment = 64;
constexpr std::size_t kDefaultVectorAlignment = 16;

template <typename T, std::size_t M, std::size_t N>
using simd_mat_t = Matrix<T, M, N, kDefaultMatrixAlignment>;

#define DECL_MAT_CLASS(Name, Type, Dim0, Dim1)             \
  class Name final : public simd_mat_t<Type, Dim0, Dim1> { \
    using Base = simd_mat_t<Type, Dim0, Dim1>;             \
    using Base::Base;                                      \
  }

DECL_MAT_CLASS(mat4, float, 4, 4);
DECL_MAT_CLASS(mat3, float, 3, 3);
DECL_MAT_CLASS(mat2, float, 2, 2);

DECL_MAT_CLASS(mat4i, int, 4, 4);
DECL_MAT_CLASS(mat3i, int, 3, 3);
DECL_MAT_CLASS(mat2i, int, 2, 2);

template <typename T, std::size_t N>
using simd_vec_t = Vector<T, N, kDefaultVectorAlignment>;

#define DECL_VEC_CLASS(Name, Type, Dim0)       \
  class Name : public simd_vec_t<Type, Dim0> { \
    using Base = simd_vec_t<Type, Dim0>;       \
    using Base::Base;                          \
  }

DECL_VEC_CLASS(vec4, float, 4);
DECL_VEC_CLASS(vec3, float, 3);
DECL_VEC_CLASS(vec2, float, 2);

DECL_VEC_CLASS(vec4i, int, 4);
DECL_VEC_CLASS(vec3i, int, 3);
DECL_VEC_CLASS(vec2i, int, 2);

namespace common {

class ISimdExecutor {
 public:
  ISimdExecutor() = default;
  virtual ~ISimdExecutor() = default;

  virtual mat2 mul(const mat2& a, const mat2& b) const = 0;

  virtual mat2i mul(const mat2i& a, const mat2i& b) const = 0;

  virtual mat3 mul(const mat3& a, const mat3& b) const = 0;

  virtual mat3i mul(const mat3i& a, const mat3i& b) const = 0;

  virtual mat4 mul(const mat4& a, const mat4& b) const = 0;

  virtual mat4i mul(const mat4i& a, const mat4i& b) const = 0;
};

}  // namespace common
}  // namespace math::simd