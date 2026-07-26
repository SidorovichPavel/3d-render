module;

#include <meta>
#include <type_traits>

export module reflection;

namespace {}  // namespace

export namespace reflection {

template <typename E>
consteval auto enumerators_of_static() {
  return std::define_static_array(std::meta::enumerators_of(^^E));
}

template <typename E>
  requires std::is_enum_v<E>
constexpr auto MaxEnumValue() noexcept {
  using result_t = std::underlying_type_t<E>;
  result_t result = std::numeric_limits<result_t>::min();

  template for (constexpr auto enumerator : enumerators_of_static<E>()) {
    result = std::max(result, static_cast<result_t>([:enumerator:]));
  }

  return result;
}

}  // namespace reflection