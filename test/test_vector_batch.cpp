#include <gtest/gtest.h>

#include <algorithm>
#include <expected>
#include <random>
#include <ranges>
#include <vector>

import math;

using math::vec4;
using math::VectorBatch;

TEST(VectorBatchTest, FixedConstructor) {
  constexpr std::array data = {vec4{1.f, 2.f, 3.f, 4.f}};
  VectorBatch batch(std::span{data});
  auto expected = std::make_tuple(1.f, 2.f, 3.f, 4.f);
  EXPECT_EQ(batch[0], expected);
}

TEST(VectorBatchTest, ToVectorBatch) {
  std::random_device rd;
  std::mt19937 gen(rd());

  const auto num = 2 * VectorBatch::Size() + VectorBatch::Size() / 2;
  std::vector<vec4> data(num);
  std::uniform_real_distribution<float> dis(1.f, 20.f);
  std::ranges::generate(
      data, [&]() { return vec4{dis(gen), dis(gen), dis(gen), dis(gen)}; });

  auto batches = math::ToBatches(data);
  EXPECT_EQ(batches.size(), 3);

  for (auto [idx, e] : data | std::views::enumerate) {
    const auto batch_idx = idx / VectorBatch::Size();
    const auto row_idx = idx % VectorBatch::Size();
    auto expected = std::make_tuple(e.x(), e.y(), e.z(), e.w());
    EXPECT_EQ(batches[batch_idx][row_idx], expected);
  }

  {
    auto expected = std::make_tuple(0.f, 0.f, 0.f, 0.f);
    auto test = batches[2][VectorBatch::Size() - 1];
    EXPECT_EQ(test, expected);
  }
}
