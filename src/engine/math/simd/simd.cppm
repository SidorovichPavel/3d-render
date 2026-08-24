module;

#include <memory>
#include <string_view>

export module math.simd;

export import math.simd.common;
import math.simd.fallback;
// import math.simd.x86_64_v2;
import math.simd.x86_64_v4;

namespace {

namespace arch {

using namespace std::string_view_literals;

constexpr auto x86_64_v2 = "x86-64-v2"sv;
constexpr auto x86_64_v3 = "x86-64-v3"sv;
constexpr auto x86_64_v4 = "x86-64-v4"sv;

}  // namespace arch

class Dispatcher {
 public:
  static auto GetInstance() noexcept -> Dispatcher& {
    static Dispatcher instance;
    return instance;
  }

  ~Dispatcher() = default;

  Dispatcher(const Dispatcher&) = delete;
  Dispatcher& operator=(const Dispatcher&) = delete;
  Dispatcher(Dispatcher&&) = delete;
  Dispatcher& operator=(Dispatcher&&) = delete;

  auto GetExecutor() noexcept -> math::simd::common::ISimdExecutor& {
    return *executor_ptr_;
  }

 private:
  Dispatcher() noexcept {
    executor_ptr_ = [] -> std::unique_ptr<math::simd::common::ISimdExecutor> {
      if (__builtin_cpu_supports("x86-64-v4"))
        return math::simd::x86_64_v4::MakeExecutor();

      // if (__builtin_cpu_supports("x86-64-v2"))
      //   return math::simd::x86_64_v2::MakeExecutor();

      return math::simd::fallback::MakeExecutor();
    }();
  }

  std::unique_ptr<math::simd::common::ISimdExecutor> executor_ptr_;
};

}  // namespace

export namespace math::simd {

auto operator*(const mat4& m1, const mat4& m2) noexcept {
  return Dispatcher::GetInstance().GetExecutor().mul(m1, m2);
}

auto operator*(const mat4i& m1, const mat4i& m2) noexcept {
  return Dispatcher::GetInstance().GetExecutor().mul(m1, m2);
}

}  // namespace math::simd
