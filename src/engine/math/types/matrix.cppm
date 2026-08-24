module;

#include <algorithm>
#include <array>
#include <cassert>
#include <functional>
#include <mdspan>
#include <ranges>

export module math.types:matrix;

using std::views::indices;

export namespace math {

template <class T, std::size_t M, std::size_t N,
          std::size_t Alignment = alignof(T)>
  requires(std::is_arithmetic_v<T> && M > 1 && N > 1)
class Matrix {
 public:
  using value_type = T;
  static constexpr auto kDim0 = M;
  static constexpr auto kDim1 = N;
  static constexpr auto kAlignment = Alignment;
  using container_type = std::array<T, M * N>;

  explicit constexpr Matrix() noexcept {
    std::ranges::fill(*this, static_cast<T>(0));
  }

  constexpr Matrix(const Matrix& other) noexcept = default;
  constexpr Matrix& operator=(const Matrix& right) noexcept = default;

  explicit constexpr Matrix(const T& val) noexcept : Matrix() {
    static_assert(M == N, "Error matrix initialize. Matrix is not quadratic.");
    for (auto i : indices(M)) accessor()[i, i] = val;
  }

  explicit constexpr Matrix(std::initializer_list<T> init_list) noexcept
      : Matrix() {
    const auto size = std::max(init_list.size(), M * N);
    for (const auto [idx, e] : init_list | std::views::enumerate) {
      if (static_cast<std::size_t>(idx) >= size) break;
      const auto i = idx / N;
      const auto j = idx % N;
      (*this)[i, j] = e;
    }
  }

  explicit constexpr Matrix(const Matrix<T, (M + 1), (N + 1)>& other) noexcept
      : Matrix() {
    for (auto&& i : std::views::iota(0u, N))
      std::ranges::copy_n(other[i], M, (*this)[i].begin());
  }

  template <std::ranges::range Rn>
  explicit constexpr Matrix(const Rn& range) noexcept : Matrix() {
    const auto size =
        std::min(Size(), static_cast<std::size_t>(std::ranges::size(range)));
    std::ranges::copy_n(std::ranges::begin(range), size, begin());
  }

  template <typename Self>
  constexpr auto operator[](this Self&& self, std::size_t i,
                            std::size_t j) noexcept -> decltype(auto) {
    return std::forward<Self>(self).accessor()[j, i];
  }

  constexpr Matrix operator-() const noexcept {
    Matrix res;
    std::ranges::transform(*this, res.begin(), std::negate());
    return res;
  }

  constexpr Matrix operator+() const noexcept { return *this; }

  constexpr Matrix& operator+=(const Matrix& right) noexcept {
    std::ranges::transform(*this, right, begin(), std::plus());
    return *this;
  }

  constexpr Matrix& operator-=(const Matrix& right) noexcept {
    std::ranges::transform(*this, right, begin(), std::minus());
    return *this;
  }

  template <class U>
  constexpr Matrix& operator*=(const U& right) noexcept {
    constexpr auto unary_mul = std::bind_front(std::multiplies(), right);
    std::ranges::transform(*this, begin(), unary_mul);
    return *this;
  }

  template <class U>
  constexpr Matrix& operator/=(const U& right) noexcept {
    const auto inv = 1.f / right;
    return *this *= inv;
  }

  template <typename Self>
  constexpr auto operator[](this Self&& self, std::size_t i) noexcept {
    std::span s{std::forward<Self>(self).data_};
    const auto width = N;
    const auto offset = i * width;
    const auto count = offset + width <= s.size() ? width : 0U;

    return s.subspan(offset, count);
  }

  template <typename Self>
  constexpr auto data(this Self&& self) noexcept {
    return std::forward<Self>(self).data_.data();
  }

  template <typename Self>
  constexpr auto begin(this Self&& self) noexcept {
    return std::forward<Self>(self).data_.begin();
  }

  template <typename Self>
  constexpr auto cbegin(this Self&& self) noexcept {
    return std::forward<Self>(self).data_.cbegin();
  }

  template <typename Self>
  constexpr auto end(this Self&& self) noexcept {
    return std::forward<Self>(self).data_.end();
  }

  template <typename Self>
  constexpr auto cend(this Self&& self) noexcept {
    return std::forward<Self>(self).data_.cend();
  }

  static constexpr auto Size() noexcept -> std::size_t { return M * N; }

 private:
  template <typename Self>
  auto accessor(this Self&& self) noexcept {
    return std::mdspan(std::forward<Self>(self).data_.data(), M, N);
  }

  alignas(Alignment) container_type data_{};
};

template <typename T, std::size_t M, std::size_t N, std::size_t Alignment>
constexpr auto RankUp(const Matrix<T, M, N, Alignment>& mat) noexcept
    -> Matrix<T, M + 1, N + 1, Alignment> {
  using result_type = Matrix<T, M + 1, N + 1, Alignment>;

  result_type res{};
  for (const auto i : std::views::indices(N)) {
    std::ranges::copy(mat[i], res[i].begin());
  }

  res[M, N] = static_cast<T>(1);

  return res;
}

template <typename T, std::size_t M, std::size_t N, std::size_t Alignment>
constexpr auto RankDown(const Matrix<T, M, N, Alignment>& mat) noexcept
    -> Matrix<T, M - 1, N - 1, Alignment> {
  using result_type = Matrix<T, M - 1, N - 1, Alignment>;

  result_type res{};
  for (const auto i : std::views::indices(N - 1)) {
    std::ranges::copy_n(mat[i].cbegin(), M - 1, res[i].begin());
  }

  return res;
}

template <typename T, std::size_t M, std::size_t N, std::size_t Alignment>
constexpr bool operator==(const Matrix<T, M, N, Alignment>& lhs,
                          const Matrix<T, M, N, Alignment>& rhs) noexcept {
  if (&lhs == &rhs) return true;
  return std::ranges::equal(lhs, rhs);
}

}  // namespace math
