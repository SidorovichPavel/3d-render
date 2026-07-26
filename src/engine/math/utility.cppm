module;

#include <utility>
#include <type_traits>

export module math:utility;

export namespace math::utility {

template <typename T, typename U = T>
using mul_result_t = decltype(std::declval<T>() * std::declval<U>());

template <class T, class U, bool Expr>
using enable_if_t =
    std::enable_if_t<std::is_same_v<T, std::remove_reference_t<U>> && Expr, U>;

}
