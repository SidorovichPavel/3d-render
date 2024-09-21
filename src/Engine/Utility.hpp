#pragma once

#include <concepts>
#include <numeric>
#include <queue>
#include <tuple>
#include <vector>

#include <tinyalgebra/math/type_decl.hpp>

#include <cassert>

namespace engine {

using Tile = ta::vec2;
using TileQueue = std::queue<Tile>;
using TileQueueGrid = std::vector<TileQueue>;

// non-owning multidimention view
template <typename T, std::size_t Dims>
class mdspan {
  static_assert(Dims != 0, "Invalid zero-dimention object");

 public:
  mdspan() noexcept : ptr_(nullptr) {
    std::ranges::fill(dims_size, static_cast<std::size_t>(0));
  }

  template <typename... Args>
  mdspan(T* ptr, Args&&... args) noexcept : ptr_(ptr) {
    static_assert(
        (std::is_same_v<std::remove_cvref_t<Args>, std::size_t> && ...),
        "fail");
    static_assert(sizeof...(Args) == Dims, "Incomplite dimention!");
    auto first = dims_size.begin();
    ((*first++ = args), ...);
  }

  ~mdspan() = default;

  template <std::ranges::range Range>
  mdspan(T* ptr, Range range) noexcept : ptr_(ptr) {
    assert(std::distance(range.begin(), range.end()) == Dims);
    std::copy(range.begin(), range.end(), dims_size.begin());
  }

  mdspan(mdspan&) = default;
  mdspan(mdspan&&) = default;

  mdspan& operator=(mdspan&) = default;
  mdspan& operator=(mdspan&&) = default;

  mdspan<T, (Dims - 1)> operator[](std::size_t offset) noexcept {
    assert(offset < dims_size[0]);
    assert(ptr_);
    std::size_t block_size =
        std::accumulate(std::next(dims_size.begin()), dims_size.end(), 1u,
                        std::multiplies<std::size_t>());
    auto ptr_with_offset = ptr_ + offset * block_size;
    return mdspan<T, (Dims - 1)>(
        ptr_with_offset,
        std::ranges::subrange(std::next(dims_size.begin()), dims_size.end()));
  }

 private:
  std::array<std::size_t, Dims> dims_size;
  T* ptr_;
};

template <typename T>
class mdspan<T, 1u> {
 public:
  template <std::ranges::range Range>
  mdspan(T* ptr, Range range) noexcept : ptr_(ptr) {
    assert(std::distance(range.begin(), range.end()) == 1u);
    dim_size = *range.begin();
  }

  ~mdspan() = default;

  mdspan(mdspan&) = default;
  mdspan(mdspan&&) = default;

  mdspan& operator=(mdspan&) = default;
  mdspan& operator=(mdspan&&) = default;

  T& operator[](std::size_t offset) noexcept {
    assert(offset < dim_size);
    assert(ptr_);
    auto ptr_with_offset = ptr_ + offset;
    return *ptr_with_offset;
  }

 private:
  T* ptr_;
  std::size_t dim_size;
};

}  // namespace engine