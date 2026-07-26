module;

#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <functional>
#include <ranges>
#include <string>

export module math:vector;

import :utility;

namespace detail {

template <typename T, typename U = T>
using declval_by_mul = math::utility::mul_result_t<T, U>;

using math::utility::enable_if_t;

}

export namespace math {

namespace swz {
constexpr size_t x = 0;
constexpr size_t y = 1;
constexpr size_t z = 2;
constexpr size_t w = 3;
}  // namespace swz

template <class T, size_t Dim, std::size_t Alignment = alignof(T)>
  requires (std::is_arithmetic_v<T> && Dim > 1)
                                    class Vector {
 public:
  using value_type = T;
  using container_type = std::array<value_type, Dim>;
  using iterator = container_type::iterator;
  using const_iterator = container_type::const_iterator;
  using this_type = Vector<T, Dim>;
  using value_ref_type = value_type&;

 private:
  alignas(Alignment) std::array<value_type, Dim> data_;

 public:
  constexpr std::pair<iterator, iterator> _get_range() noexcept {
    return {data_, data_ + Dim};
  }

  constexpr std::pair<const_iterator, const_iterator> _get_range()
      const noexcept {
    return {data_, data_ + Dim};
  }

  template <class Fn>
  constexpr Vector transform_to_new(Fn&& _Func) const noexcept {
    Vector result;
    std::transform(begin(), end(), result.begin(), _Func);
    return result;
  }

 public:
  constexpr Vector() noexcept { std::fill(begin(), end(), static_cast<T>(0)); }

  constexpr Vector(T val) noexcept { std::ranges::fill(data_, val); }

  template <class U>
  constexpr Vector(Vector<U, Dim>& other) noexcept {
    std::copy(other.begin(), other.end(), begin());
  }

  template <class U>
  constexpr Vector(const Vector<U, Dim>& other) noexcept {
    std::copy(other.begin(), other.end(), begin());
  }

  template <class U>
  constexpr Vector(Vector<U, Dim>&& other) noexcept {
    std::copy(other.begin(), other.end(), begin());
  }

  Vector(std::initializer_list<T> init_list) noexcept {
    if (init_list.size() <= Dim)
      std::copy(init_list.begin(), init_list.end(), begin());
  }

  template <class U, class V>
  constexpr Vector(Vector<U, (Dim - 1)>& vec, V val) noexcept {
    auto first = begin();
    std::copy(vec.begin(), vec.end(), std::ref(first));
    *first = static_cast<T>(val);
  }

  template <class U, class V>
  constexpr Vector(const Vector<U, (Dim - 1)>& vec, V val) noexcept {
    auto first = begin();
    std::copy(vec.begin(), vec.end(), std::ref(first));
    *first = static_cast<T>(val);
  }

  template <class U, class V>
  constexpr Vector(Vector<U, (Dim - 1)>&& vec, V val) noexcept {
    auto first = begin();
    std::copy(vec.begin(), vec.end(), std::ref(first));
    *first = static_cast<T>(val);
  }

  template <class U>
  constexpr Vector(Vector<U, (Dim + 1)>& vec) noexcept {
    std::copy_n(vec.begin(), Dim, begin());
  }

  template <class U>
  constexpr Vector(const Vector<U, (Dim + 1)>& vec) noexcept {
    std::copy_n(vec.begin(), Dim, begin());
  }

  template <class U>
  constexpr Vector(Vector<U, (Dim + 1)>&& vec) noexcept {
    std::copy_n(vec.begin(), Dim, begin());
  }

  template <class... Args>
  constexpr Vector(Args&&... args) noexcept {
    static_assert(sizeof...(Args) == Dim, "Too many indices for construct");
    static_assert(((std::is_arithmetic_v<std::remove_cvref_t<Args>>) && ...),
                  "Incorrect arguments type");

    auto it = begin();
    ((*it++ = static_cast<T>(args)), ...);
  }

  constexpr ~Vector() {}

  static constexpr size_t size() noexcept { return Dim; }

  template <class U>
  this_type& operator=(const Vector<U, Dim>& right) noexcept {
    std::copy(right.begin(), right.end(), begin());
    return *this;
  }

  template <class U>
  this_type& operator=(Vector<U, Dim>&& right) noexcept {
    std::copy(right.begin(), right.end(), begin());
    return *this;
  }

  constexpr this_type operator-() const noexcept {
    return transform_to_new([](auto e) { return -e; });
  }

  constexpr this_type operator+() const noexcept { return *this; }

  value_ref_type operator[](size_t idx) {
    assert(idx < Dim);
    return data_[idx];
  }

  value_type operator[](size_t idx) const {
    assert(idx < Dim);
    return data_[idx];
  }

  template <class U>
  constexpr this_type& operator+=(const Vector<U, Dim>& right) noexcept {
    std::transform(begin(), end(), right.begin(), begin(),
                   [](auto a, auto b) { return static_cast<T>(a + b); });
    return *this;
  }

  template <class U>
  constexpr this_type& operator-=(const Vector<U, Dim>& right) noexcept {
    std::transform(begin(), end(), right.begin(), begin(),
                   [](auto a, auto b) { return static_cast<T>(a - b); });
    return *this;
  }

  template <class U>
  constexpr this_type& operator+=(const U& right) noexcept {
    std::transform(begin(), end(), begin(),
                   [&](auto a) { return static_cast<T>(a + right); });
    return *this;
  }

  template <class U>
  constexpr this_type& operator-=(const U& right) noexcept {
    std::transform(begin(), end(), begin(),
                   [&](auto a) { return static_cast<T>(a - right); });
    return *this;
  }

  template <class U>
  constexpr this_type& operator*=(const U& right) noexcept {
    std::transform(begin(), end(), begin(),
                   [&right](auto a) { return static_cast<T>(a * right); });
    return *this;
  }

  template <class U>
  constexpr this_type& operator/=(const U& right) noexcept {
    auto rright = 1.f / right;
    std::transform(begin(), end(), begin(),
                   [&rright](auto a) { return static_cast<T>(a * rright); });
    return *this;
  }

  constexpr iterator begin() noexcept { return data_; }
  constexpr iterator end() noexcept { return data_ + Dim; }
  constexpr const_iterator begin() const noexcept { return data_; }
  constexpr const_iterator end() const noexcept { return data_ + Dim; }

  constexpr const_iterator cbegin() noexcept { return data_; }
  constexpr const_iterator cend() noexcept { return data_ + Dim; }
  constexpr const_iterator cbegin() const noexcept { return data_; }
  constexpr const_iterator cend() const noexcept { return data_ + Dim; }

  constexpr value_type* data() noexcept { return data_; }

  constexpr const value_type* data() const noexcept { return data_; }

  constexpr float length() const noexcept {
    return std::sqrt(static_cast<float>(dot2()));
  }

  constexpr value_type dot2() const noexcept {
    T res = static_cast<T>(0);
    for (auto& e : data_) res += e * e;
    return res;
  }

  std::string to_string() const {
    std::string result("{");
    bool not_first_value = false;
    for (auto& e : data_) {
      if (not_first_value)
        result += ", ";
      else
        not_first_value = true;

      result += std::to_string(e);
    }
    result += "}";
    return result;
  }

  template <class U = T>
  constexpr detail::enable_if_t<T, U&, (Dim > 0)> x() noexcept {
    return data_[0];
  }

  template <class U = T>
  constexpr detail::enable_if_t<T, U, (Dim > 0)> x() const noexcept {
    return data_[0];
  }

  template <class U = T>
  constexpr detail::enable_if_t<T, U&, (Dim > 1)> y() noexcept {
    return data_[1];
  }

  template <class U = T>
  constexpr detail::enable_if_t<T, U, (Dim > 1)> y() const noexcept {
    return data_[1];
  }

  template <class U = T>
  constexpr detail::enable_if_t<T, U&, (Dim > 2)> z() noexcept {
    return data_[2];
  }

  template <class U = T>
  constexpr detail::enable_if_t<T, U, (Dim > 2)> z() const noexcept {
    return data_[2];
  }

  template <class U = T>
  constexpr detail::enable_if_t<T, U&, (Dim > 3)> w() noexcept {
    return data_[3];
  }

  template <class U = T>
  constexpr detail::enable_if_t<T, U, (Dim > 3)> w() const noexcept {
    return data_[3];
  }

  template <class U = T>
  constexpr detail::enable_if_t<T, U&, (Dim > 0)> r() noexcept {
    return data_[0];
  }

  template <class U = T>
  constexpr detail::enable_if_t<T, U, (Dim > 0)> r() const noexcept {
    return data_[0];
  }

  template <class U = T>
  constexpr detail::enable_if_t<T, U&, (Dim > 1)> g() noexcept {
    return data_[1];
  }

  template <class U = T>
  constexpr detail::enable_if_t<T, U, (Dim > 1)> g() const noexcept {
    return data_[1];
  }

  template <class U = T>
  constexpr detail::enable_if_t<T, U&, (Dim > 2)> b() noexcept {
    return data_[2];
  }

  template <class U = T>
  constexpr detail::enable_if_t<T, U, (Dim > 2)> b() const noexcept {
    return data_[2];
  }

  template <class U = T>
  constexpr detail::enable_if_t<T, U&, (Dim > 3)> a() noexcept {
    return data_[3];
  }

  template <class U = T>
  constexpr detail::enable_if_t<T, U, (Dim > 3)> a() const noexcept {
    return data_[3];
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
                         const Vector<U, Dim>& u) noexcept
    -> Vector<decltype(detail::declval_by_mul<T, U>()), Dim> {
  using return_type = decltype(detail::declval_by_mul<T, U>());

  Vector<return_type, Dim> result;
  auto [vbegin, vend] = v._get_range();
  auto [ubegin, uend] = u._get_range();
  auto [rbegin, rend] = result._get_range();

  std::transform(vbegin, vend, ubegin, rbegin, [](auto a, auto b) {
    return static_cast<return_type>(a + b);
  });
  return result;
}

template <class T, class U, size_t Dim>
constexpr auto operator-(const Vector<T, Dim>& v,
                         const Vector<U, Dim>& u) noexcept
    -> Vector<decltype(detail::declval_by_mul<T, U>()), Dim> {
  using return_type = decltype(detail::declval_by_mul<T, U>());

  Vector<return_type, Dim> result;
  auto [vbegin, vend] = v._get_range();
  auto [ubegin, uend] = u._get_range();
  auto [rbegin, rend] = result._get_range();

  std::transform(vbegin, vend, ubegin, rbegin, [](auto a, auto b) {
    return static_cast<return_type>(a - b);
  });
  return result;
}

template <class T, class U, size_t Dim>
constexpr std::enable_if_t<std::is_arithmetic_v<U>, Vector<T, Dim>> operator*(
    const Vector<T, Dim>& vec, U val) noexcept {
  return vec.transform_to_new(
      [val](auto e) { return static_cast<T>(val * e); });
}

template <class T, class U, size_t Dim>
constexpr std::enable_if_t<std::is_arithmetic_v<U>, Vector<T, Dim>> operator/(
    const Vector<T, Dim>& vec, U val) noexcept {
  auto rval = static_cast<U>(1) / val;
  return vec.transform_to_new(
      [rval](auto e) { return static_cast<T>(e * rval); });
}

template <class T, class U, size_t Dim>
constexpr std::enable_if_t<std::is_arithmetic_v<U>, Vector<T, Dim>> operator*(
    U val, const Vector<T, Dim>& vec) noexcept {
  return vec.transform_to_new(
      [val](auto e) { return static_cast<T>(val * e); });
}

template <class T, class U, size_t Dim>
constexpr std::enable_if_t<std::is_arithmetic_v<U>, Vector<T, Dim>> operator/(
    U val, const Vector<T, Dim>& vec) noexcept {
  return vec.transform_to_new(
      [val](auto e) { return static_cast<T>(val / e); });
}

Vector<float, 4> operator+(const Vector<float, 4>& u,
                           const Vector<float, 4>& v) noexcept;
Vector<float, 4> operator-(const Vector<float, 4>& u,
                           const Vector<float, 4>& v) noexcept;

Vector<float, 4> operator*(const Vector<float, 4>& vec,
                           const float val) noexcept;
Vector<float, 4> operator*(const float val,
                           const Vector<float, 4>& vec) noexcept;
Vector<float, 4> operator/(const Vector<float, 4>& vec,
                           const float val) noexcept;
Vector<float, 4> operator/(const float val,
                           const Vector<float, 4>& vec) noexcept;

}  // namespace math
