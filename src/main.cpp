#include <chrono>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <mdspan>
#include <print>
#include <ranges>
#include <simd>
#include <sstream>
#include <vector>

#include <boost/program_options.hpp>

#include <spdlog/spdlog.h>

import window;
import universal;
import math;

namespace {

std::string DumpHelpText(
    const boost::program_options::options_description& desc) {
  std::stringstream ss;
  ss << desc;
  return ss.str();
}

auto DescribeSelf() {
  namespace po = boost::program_options;

  // Declare the supported options.
  po::options_description desc("Allowed options");
  desc.add_options()("help", "produce help message")(
      "compression", po::value<int>(), "set compression level");

  return desc;
}

auto ParseArgs(int ac, char** av,
               const boost::program_options::options_description& desc) {
  namespace po = boost::program_options;

  po::variables_map vm;
  po::store(po::parse_command_line(ac, av, desc), vm);
  po::notify(vm);

  return vm;
}

}  // namespace

int main(int argc, char** argv) {
  auto description = DescribeSelf();
  auto arg_map = ParseArgs(argc, argv, description);

  if (arg_map.count("help")) {
    std::println("{}", DumpHelpText(description));
  }

  spdlog::set_level(spdlog::level::debug);

  try {
    constexpr auto S = 0.9f;
    constexpr auto L = 0.5f;

    constexpr auto kWidth = 800;
    constexpr auto kHeight = 600;

    mfb::IOContext io_context;
    spdlog::debug("Size of object IOContext: {}", sizeof(io_context));

    auto window = mfb::MakeWindow(io_context, "Software 3D", kWidth, kHeight,
                                  mfb::WindowFlag::kResizable);

    std::vector<uint32_t> buffer(kWidth * kHeight);
    const auto colors = std::mdspan(buffer.data(), kHeight, kWidth);

    auto frame_time_point = std::chrono::steady_clock::now();
    std::chrono::nanoseconds dt{0};
    std::chrono::milliseconds ms{0};

    do {
      // TODO: add some fancy rendering to the buffer of size 800 * 600

      dt = std::chrono::steady_clock::now() - frame_time_point;
      frame_time_point = std::chrono::steady_clock::now();

      ms += std::chrono::duration_cast<std::chrono::milliseconds>(dt);

      for (auto j : std::views::iota(0, kWidth)) {
        const auto H = ((j + ms.count()) % kWidth) / static_cast<float>(kWidth);
        using universal::color_scheme::HSLtoRGB2;
        const auto color = std::apply(mfb::MakeColor, HSLtoRGB2(H, S, L));

        for (auto i : std::views::iota(0, 120)) {
          colors[i, j] = color;
        }
      }

      auto state = window.Update(buffer.data(), kWidth, kHeight);

      if (state != mfb::UpdateState::kOk) break;

    } while (!window.ShouldClose());

  } catch (mfb::MfbRuntimeError& err) {
    spdlog::error("Exception: {} at\n{}", err.What(), err.Where());
  } catch ([[maybe_unused]] std::exception& e) {
    spdlog::error("Exception: {}", e.what());
  }

  return 0;
}
