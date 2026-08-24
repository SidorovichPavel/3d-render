module;

#include <type_traits>
#include <utility>

export module math.types:utility;

export namespace math::utility {

template <typename T, typename U = T>
using mul_result_t = decltype(std::declval<T>() * std::declval<U>());

template <typename T, typename U>
using binary_arithmetic_result_t =
    std::conditional_t<std::same_as<T, mul_result_t<T, U>>, T, U>;

}  // namespace math::utility
