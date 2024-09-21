#pragma once

#include <future>
#include <stdexcept>
#include <vector>

#include <threadpool/threadpool.hpp>
#include <tinyalgebra/math/type_decl.hpp>

namespace detail {

template <typename Iter>
concept RandomAccessIterator = requires(Iter it) {
  { it + 1 } -> std::same_as<Iter>;
  { it - 1 } -> std::same_as<Iter>;
  { it += 1 } -> std::same_as<Iter&>;
  { it -= 1 } -> std::same_as<Iter&>;
  { it - it } -> std::convertible_to<std::ptrdiff_t>;
  { it + std::ptrdiff_t(1) } -> std::same_as<Iter>;
  { std::distance(it, it) } -> std::same_as<std::ptrdiff_t>;
};

template <RandomAccessIterator Iter>
class ref_to_row {
 public:
  using value_type = Iter::value_type;

  ref_to_row(Iter iter, size_t max_size);
  value_type& operator[](size_t offset);

 private:
  Iter row_iter_;
  size_t max_size_;
};

template <RandomAccessIterator Iter>
ref_to_row<Iter>::ref_to_row(Iter iter, size_t max_size)
    : row_iter_(iter), max_size_(max_size) {}

template <RandomAccessIterator Iter>
ref_to_row<Iter>::value_type& ref_to_row<Iter>::operator[](size_t offset) {
  if (offset >= max_size_) throw std::out_of_range("Out of row range");
  return row_iter_[offset];
}

}  // namespace detail

class ScreenBuffer final {
 public:
  ScreenBuffer(size_t width, size_t height, ta::vec3 default_color);

  detail::ref_to_row<std::vector<ta::vec3>::iterator> operator[](size_t offset);

  detail::ref_to_row<std::vector<float>::iterator> z(size_t offset);

  size_t size();

  void clear();

  void* data();

  std::vector<float> as_floats() const;

 private:
  size_t width_, height_;
  std::vector<ta::vec3> buffer_;
  ta::vec3 default_color_;
  std::vector<float> zbuffer_;
};