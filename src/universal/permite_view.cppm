module;

#include <concepts>
#include <functional>
#include <iterator>
#include <ranges>
#include <type_traits>
#include <utility>

export module universal:permite_view;

export namespace universal::views {

/// @brief A view that reorders elements using an indexer function
/// @tparam V The underlying view type (must be random access)
/// @tparam F The indexer function type
///
/// The indexer function takes a position in the permuted view and returns
/// the corresponding index in the underlying range.
///
/// Example:
/// @code
/// std::vector<int> data = {1, 2, 3, 4, 5};
/// auto reversed = permute_view(data, [](std::size_t pos) {
///     return data.size() - 1 - pos;
/// });
/// // reversed: {5, 4, 3, 2, 1}
/// @endcode
template <std::ranges::random_access_range V, std::copy_constructible F>
  requires std::ranges::view<V> && std::invocable<F&, std::size_t> &&
           std::convertible_to<std::invoke_result_t<F&, std::size_t>,
                               std::size_t>
class permute_view : public std::ranges::view_interface<permute_view<V, F>> {
 public:
  /// @brief Iterator type for permute_view
  class iterator;

  /// @brief Sentinel type for permute_view
  class sentinel;

 private:
  V base_;
  F indexer_;

 public:
  /// @brief Default constructor
  permute_view() = default;

  /// @brief Construct from a view and indexer function
  /// @param base The underlying view
  /// @param indexer The indexer function
  constexpr permute_view(V base, F indexer)
      : base_(std::move(base)), indexer_(std::move(indexer)) {}

  /// @brief Copy constructor
  constexpr permute_view(const permute_view&) = default;

  /// @brief Move constructor
  constexpr permute_view(permute_view&&) = default;

  /// @brief Copy assignment
  constexpr permute_view& operator=(const permute_view&) = default;

  /// @brief Move assignment
  constexpr permute_view& operator=(permute_view&&) = default;

  /// @brief Destructor
  ~permute_view() = default;

  /// @brief Get the beginning iterator
  /// @return Iterator to the first element
  [[nodiscard]] constexpr auto begin() { return iterator{*this, 0}; }

  /// @brief Get the beginning iterator (const overload)
  /// @return Iterator to the first element
  [[nodiscard]] constexpr auto begin() const
    requires std::ranges::random_access_range<const V>
  {
    return iterator{*this, 0};
  }

  /// @brief Get the end iterator
  /// @return Iterator to one past the last element
  [[nodiscard]] constexpr auto end() { return iterator{*this, size()}; }

  /// @brief Get the end iterator (const overload)
  /// @return Iterator to one past the last element
  [[nodiscard]] constexpr auto end() const
    requires std::ranges::random_access_range<const V>
  {
    return iterator{*this, size()};
  }

  /// @brief Get the size of the view
  /// @return Number of elements in the view
  [[nodiscard]] constexpr auto size()
    requires std::ranges::sized_range<V>
  {
    return std::ranges::size(base_);
  }

  /// @brief Get the size of the view (const overload)
  /// @return Number of elements in the view
  [[nodiscard]] constexpr auto size() const
    requires std::ranges::sized_range<const V>
  {
    return std::ranges::size(base_);
  }

  /// @brief Access element at position
  /// @param pos Position in the permuted view
  /// @return Reference to the element
  [[nodiscard]] constexpr auto operator[](std::size_t pos)
    requires std::ranges::random_access_range<V>
  {
    std::size_t source_idx = indexer_(pos);
    return base_[source_idx];
  }

  /// @brief Access element at position (const overload)
  /// @param pos Position in the permuted view
  /// @return Const reference to the element
  [[nodiscard]] constexpr auto operator[](std::size_t pos) const
    requires std::ranges::random_access_range<const V>
  {
    std::size_t source_idx = indexer_(pos);
    return base_[source_idx];
  }

  /// @brief Get the underlying view
  /// @return Reference to the underlying view
  [[nodiscard]] constexpr auto base() const& noexcept -> const V& {
    return base_;
  }

  /// @brief Get the underlying view (rvalue overload)
  /// @return The underlying view
  [[nodiscard]] constexpr auto base() && noexcept -> V {
    return std::move(base_);
  }

  /// @brief Get the indexer function
  /// @return Reference to the indexer function
  [[nodiscard]] constexpr auto indexer() const& noexcept -> const F& {
    return indexer_;
  }

  /// @brief Get the indexer function (rvalue overload)
  /// @return The indexer function
  [[nodiscard]] constexpr auto indexer() && noexcept -> F {
    return std::move(indexer_);
  }
};

/// @brief Iterator for permute_view
template <std::ranges::random_access_range V, std::copy_constructible F>
  requires std::ranges::view<V> && std::invocable<F&, std::size_t> &&
           std::convertible_to<std::invoke_result_t<F&, std::size_t>,
                               std::size_t>
class permute_view<V, F>::iterator {
 public:
  using iterator_category = std::random_access_iterator_tag;
  using value_type = std::ranges::range_value_t<V>;
  using difference_type = std::ranges::range_difference_t<V>;
  using reference = std::ranges::range_reference_t<V>;
  using pointer = std::add_pointer_t<value_type>;

 private:
  const permute_view* parent_ = nullptr;
  std::size_t pos_ = 0;

 public:
  /// @brief Default constructor
  iterator() = default;

  /// @brief Construct from parent view and position
  /// @param parent Pointer to the parent permute_view
  /// @param pos Position in the permuted view
  constexpr iterator(const permute_view& parent, std::size_t pos)
      : parent_(&parent), pos_(pos) {}

  /// @brief Dereference operator
  /// @return Reference to the element at current position
  [[nodiscard]] constexpr reference operator*() const {
    std::size_t source_idx = parent_->indexer_(pos_);
    return (*parent_).base_[source_idx];
  }

  /// @brief Arrow operator
  /// @return Pointer to the element at current position
  [[nodiscard]] constexpr pointer operator->() const
    requires std::is_pointer_v<std::ranges::iterator_t<V>> ||
             requires { std::ranges::iterator_t<V>::operator->(); }
  {
    return &**this;
  }

  /// @brief Subscript operator
  /// @param n Offset from current position
  /// @return Reference to the element at offset position
  [[nodiscard]] constexpr reference operator[](difference_type n) const {
    return *(*this + n);
  }

  /// @brief Pre-increment operator
  /// @return Reference to this iterator
  constexpr iterator& operator++() {
    ++pos_;
    return *this;
  }

  /// @brief Post-increment operator
  /// @return Copy of iterator before increment
  constexpr iterator operator++(int) {
    iterator tmp = *this;
    ++pos_;
    return tmp;
  }

  /// @brief Pre-decrement operator
  /// @return Reference to this iterator
  constexpr iterator& operator--() {
    --pos_;
    return *this;
  }

  /// @brief Post-decrement operator
  /// @return Copy of iterator before decrement
  constexpr iterator operator--(int) {
    iterator tmp = *this;
    --pos_;
    return tmp;
  }

  /// @brief Addition assignment operator
  /// @param n Offset to add
  /// @return Reference to this iterator
  constexpr iterator& operator+=(difference_type n) {
    pos_ += n;
    return *this;
  }

  /// @brief Subtraction assignment operator
  /// @param n Offset to subtract
  /// @return Reference to this iterator
  constexpr iterator& operator-=(difference_type n) {
    pos_ -= n;
    return *this;
  }

  /// @brief Addition operator
  /// @param n Offset to add
  /// @return New iterator at offset position
  [[nodiscard]] constexpr iterator operator+(difference_type n) const {
    iterator tmp = *this;
    tmp += n;
    return tmp;
  }

  /// @brief Subtraction operator
  /// @param n Offset to subtract
  /// @return New iterator at offset position
  [[nodiscard]] constexpr iterator operator-(difference_type n) const {
    iterator tmp = *this;
    tmp -= n;
    return tmp;
  }

  /// @friend Addition operator (left side)
  /// @param n Offset to add
  /// @param it Iterator to add to
  /// @return New iterator at offset position
  [[nodiscard]] friend constexpr iterator operator+(difference_type n,
                                                    const iterator& it) {
    return it + n;
  }

  /// @brief Difference operator
  /// @param other Other iterator
  /// @return Difference between iterators
  [[nodiscard]] constexpr difference_type operator-(
      const iterator& other) const {
    return static_cast<difference_type>(pos_) -
           static_cast<difference_type>(other.pos_);
  }

  /// @brief Equality comparison operator
  /// @param other Other iterator
  /// @return true if iterators are equal
  [[nodiscard]] constexpr bool operator==(const iterator& other) const {
    return pos_ == other.pos_;
  }

  /// @brief Three-way comparison operator
  /// @param other Other iterator
  /// @return Comparison result
  [[nodiscard]] constexpr auto operator<=>(const iterator& other) const {
    return pos_ <=> other.pos_;
  }

  /// @brief Get current position
  /// @return Current position in the permuted view
  [[nodiscard]] constexpr std::size_t position() const noexcept { return pos_; }
};

/// @brief Range adaptor closure for permute_view
namespace detail {

struct permute_fn {
  /// @brief Create a permute_view with the given indexer
  /// @param indexer The indexer function
  /// @return A range adaptor closure
  template <std::copy_constructible F>
  [[nodiscard]] constexpr auto operator()(F&& indexer) const {
    return [indexer =
                std::forward<F>(indexer)]<std::ranges::viewable_range R>(R&& r)
      requires std::ranges::random_access_range<std::views::all_t<R>>
    {
      return permute_view<std::views::all_t<R>, std::decay_t<F>>(
          std::views::all(std::forward<R>(r)), std::move(indexer));
    };
  }
};

}  // namespace detail

/// @brief Range adaptor for permute_view
inline constexpr detail::permute_fn permute;

}  // namespace universal::views
