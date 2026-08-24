module;

#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <functional>
#include <ranges>

export module math.types:vector;

export import :utility;

export namespace math {

namespace swz {
constexpr size_t x = 0;
constexpr size_t y = 1;
constexpr size_t z = 2;
constexpr size_t w = 3;
}  // namespace swz

template <class T, size_t Dim, std::size_t Alignment = alignof(T)>
  requires(std::is_arithmetic_v<T> && Dim > 1)
class Vector {
 public:
  using container_type = std::array<T, Dim>;

 private:
  alignas(Alignment) container_type data_;

 public:
  constexpr Vector() noexcept : Vector(0) {}

  explicit constexpr Vector(T val) noexcept { std::ranges::fill(data_, val); }

  constexpr Vector(const Vector& other) noexcept {
    std::ranges::copy(other, begin());
  }

  explicit constexpr Vector(std::initializer_list<T> init_list) noexcept
      : Vector(0) {
    const auto size = std::min(Dim, init_list.size());
    std::ranges::copy_n(init_list.begin(), size, begin());
  }

  template <class U, class V>
  constexpr Vector(const Vector<U, (Dim - 1)>& vec, V val) noexcept {
    auto it = begin();
    std::ranges::copy(vec, std::ref(it));
    *it = static_cast<T>(val);
  }

  template <class U>
  explicit constexpr Vector(const Vector<U, (Dim + 1)>& vec) noexcept {
    std::ranges::copy_n(vec, Dim, begin());
  }

  constexpr auto operator<=>(const Vector&) const = default;

  static constexpr size_t size() noexcept { return Dim; }

  constexpr Vector& operator=(const Vector& right) noexcept {
    if (this != &right) {
      std::ranges::copy(right, begin());
    }

    return *this;
  }

  constexpr Vector operator-() const noexcept {
    Vector res;
    std::ranges::transform(*this, res.begin(), std::negate());
    return res;
  }

  constexpr Vector operator+() const noexcept { return *this; }

  template <typename Self>
  constexpr auto operator[](this Self&& self, size_t idx) -> decltype(auto) {
    assert(idx < Dim);
    return std::forward<Self>(self).data_[idx];
  }

  template <typename U = T>
  constexpr Vector& operator+=(const Vector<U, Dim>& right) noexcept {
    std::ranges::transform(*this, right, begin(), std::plus<T>());
    return *this;
  }

  template <typename U = T>
  constexpr Vector& operator-=(const Vector<U, Dim>& right) noexcept {
    std::ranges::transform(*this, right, begin(), std::minus<T>());
    return *this;
  }

  template <class U>
  constexpr Vector& operator*=(const U& right) noexcept {
    std::ranges::transform(*this, begin(),
                           std::bind_front(std::multiplies<T>(), right));
    return *this;
  }

  template <class U>
  constexpr Vector& operator/=(const U& right) noexcept {
    auto inv = 1.f / right;
    *this *= inv;
    return *this;
  }

  template <typename Self>
  constexpr auto begin(this Self&& self) noexcept {
    return std::forward<Self>(self).data_.begin();
  }

  template <typename Self>
  constexpr auto end(this Self&& self) noexcept {
    return std::forward<Self>(self).data_.end();
  }

  template <typename Self>
  constexpr auto cbegin(this Self&& self) noexcept {
    return std::forward<Self>(self).data_.cbegin();
  }

  template <typename Self>
  constexpr auto cend(this Self&& self) noexcept {
    return std::forward<Self>(self).data_.cend();
  }

  template <typename Self>
  constexpr auto data(this Self&& self) noexcept {
    return std::forward<Self>(self).data_.data();
  }

  template <typename Result = T>
  constexpr Result length() const noexcept {
    return static_cast<Result>(std::sqrt(static_cast<float>(dot2())));
  }

  constexpr T dot2() const noexcept {
    T res = static_cast<T>(0);
    for (auto& e : data_) res += e * e;
    return res;
  }

  template <typename Self, typename U = T>
  constexpr auto x(this Self&& self) noexcept -> decltype(auto)
    requires(Dim >= 1)
  {
    return std::forward<Self>(self).data_[0];
  }

  template <typename Self, typename U = T>
  constexpr auto y(this Self&& self) noexcept -> decltype(auto)
    requires(Dim >= 2)
  {
    return std::forward<Self>(self).data_[1];
  }

  template <typename Self, typename U = T>
  constexpr auto z(this Self&& self) noexcept -> decltype(auto)
    requires(Dim >= 3)
  {
    return std::forward<Self>(self).data_[2];
  }

  template <typename Self, typename U = T>
  constexpr auto w(this Self&& self) noexcept -> decltype(auto)
    requires(Dim >= 4)
  {
    return std::forward<Self>(self).data_[3];
  }

  template <typename Self, typename U = T>
  constexpr auto r(this Self&& self) noexcept -> decltype(auto)
    requires(Dim >= 1)
  {
    return std::forward<Self>(self).data_[0];
  }

  template <typename Self, typename U = T>
  constexpr auto g(this Self&& self) noexcept -> decltype(auto)
    requires(Dim >= 2)
  {
    return std::forward<Self>(self).data_[1];
  }

  template <typename Self, typename U = T>
  constexpr auto b(this Self&& self) noexcept -> decltype(auto)
    requires(Dim >= 3)
  {
    return std::forward<Self>(self).data_[2];
  }

  template <typename Self, typename U = T>
  constexpr auto a(this Self&& self) noexcept -> decltype(auto)
    requires(Dim >= 4)
  {
    return std::forward<Self>(self).data_[3];
  }

  template <size_t... Indices>
  constexpr auto swizzle() const noexcept {
    // static_assert(sizeof...(Indices) <= Dim, "Too many indices for swizzle");
    static_assert(((Indices < Dim) && ...), "Invalid swizzle index");

    Vector<T, sizeof...(Indices)> result;
    size_t idx = 0;
    ((result[idx++] = (*this)[Indices]), ...);
    return result;
  }
};

template <class T, class U, size_t Dim>
constexpr auto operator+(const Vector<T, Dim>& v,
                         const Vector<U, Dim>& u) noexcept {
  using result_type = utility::binary_arithmetic_result_t<T, U>;

  Vector<result_type, Dim> result(v);
  return result += u;
}

template <class T, class U, size_t Dim>
constexpr auto operator-(const Vector<T, Dim>& v,
                         const Vector<U, Dim>& u) noexcept {
  using result_type = utility::binary_arithmetic_result_t<T, U>;

  Vector<result_type, Dim> result(v);
  return result -= u;
}

template <class T, class U, size_t Dim>
  requires std::is_arithmetic_v<U>
constexpr auto operator*(const Vector<T, Dim>& vec, U val) noexcept {
  using result_type = utility::binary_arithmetic_result_t<T, U>;

  Vector<result_type, Dim> result(vec);
  return result *= val;
}

template <class T, class U, size_t Dim>
  requires std::is_arithmetic_v<U>
constexpr auto operator/(const Vector<T, Dim>& vec, U val) noexcept {
  using result_type = utility::binary_arithmetic_result_t<T, U>;

  Vector<result_type, Dim> result(vec);
  return result /= val;
}

template <class T, class U, size_t Dim>
  requires std::is_arithmetic_v<U>
constexpr auto operator*(U val, const Vector<T, Dim>& vec) noexcept {
  return vec * val;
}

}  // namespace math
