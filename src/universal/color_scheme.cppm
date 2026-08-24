module;

#include <algorithm>
#include <cstdint>
#include <functional>
#include <limits>
#include <tuple>

export module universal:color_scheme;

namespace {

constexpr auto operator""_u16(unsigned long long val) -> std::uint16_t {
  return static_cast<uint16_t>(val);
}

}  // namespace

export namespace universal::color_scheme {

float HueToRgb(float p, float q, float t) {
  if (t < 0.0f) t += 1.0f;
  if (t > 1.0f) t -= 1.0f;

  if (t < 1.0f / 6.0f) return p + (q - p) * 6.0f * t;
  if (t < 1.0f / 2.0f) return q;
  if (t < 2.0f / 3.0f) return p + (q - p) * (2.0f / 3.0f - t) * 6.0f;

  return p;
}

auto HSLtoRGB(float h, float s, float l) -> std::tuple<float, float, float> {
  if (s < std::numeric_limits<float>::epsilon()) {
    return {l, l, l};
  }

  const float q = (l < 0.5f) ? l * (1.0f + s) : l + s - l * s;
  const float p = 2.0f * l - q;

  auto toRgb = std::bind_front(&HueToRgb, p, q);

  constexpr auto r3 = 1.0f / 3.0f;

  return {toRgb(h + r3), toRgb(h), toRgb(h - r3)};
}

auto HSLtoRGB2(float h, float s, float l) {
  auto [r, g, b] = HSLtoRGB(h, s, l);

  const auto fix = [](float val) {
    const auto c = static_cast<std::uint16_t>(val * 255);
    return static_cast<std::uint8_t>(std::clamp(c, 0_u16, 255_u16));
  };

  return std::make_tuple(fix(r), fix(g), fix(b));
}

}  // namespace universal::color_scheme
