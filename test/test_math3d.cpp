#include <gtest/gtest.h>

import math;

using math::Matrix;
using math::Vector;

TEST(Math3D, Multiply) {
  using mat4 = Matrix<float, 4, 4>;
  using vec4 = Vector<float, 4>;

  const vec4 a{1.f, 2.f, 3.f, 1.f};
  const vec4 b = a;
  mat4 m(1.f);
  EXPECT_EQ(m * a, b);

  m = mat4(2.f);
  const auto c = b * 2.f;
  const auto r = m * a;
  EXPECT_EQ(c, r);
}
