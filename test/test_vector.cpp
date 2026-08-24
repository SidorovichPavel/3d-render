#include <gtest/gtest.h>

import math;

using math::Vector;

TEST(VectorTest, Constructor) {
  using vec4 = Vector<float, 4>;
  using vec4i = Vector<int, 4>;
  EXPECT_EQ(vec4(), vec4(0.f));
  EXPECT_EQ(vec4i(), vec4i(0));
  {
    constexpr vec4 target{1.f, 1.f, 1.f, 1.f};
    EXPECT_EQ(vec4(1.f), target);
  }
  {
    constexpr vec4i target{1, 1, 1, 1};
    EXPECT_EQ(vec4i(1), target);
  }
}

TEST(VectorTest, Assignment) {
  using vec4i = Vector<int, 4>;

  vec4i vec;
  constexpr vec4i target(5);
  EXPECT_EQ(vec = target, target);
}

TEST(VectorTest, Sum) {
  using vec4i = Vector<int, 4>;

  vec4i vec1(2), vec2(3), vec3(5);
  EXPECT_EQ(vec1 + vec2, vec3);
  EXPECT_EQ(vec1 += vec2, vec3);
  vec1 = vec4i{1, 2, 3, 4};
  vec2 = vec4i{4, 3, 2, 1};
  EXPECT_EQ(vec1 + vec2, vec3);
}

TEST(VectorTest, Access) {
  using vec4i = Vector<int, 4>;

  constexpr vec4i vec1{1, 2, 3, 4};

  EXPECT_EQ(vec1[0], 1);
  EXPECT_EQ(vec1[1], 2);
  EXPECT_EQ(vec1[2], 3);
  EXPECT_EQ(vec1[3], 4);

  EXPECT_EQ(vec1.x(), 1);
  EXPECT_EQ(vec1.y(), 2);
  EXPECT_EQ(vec1.z(), 3);
  EXPECT_EQ(vec1.w(), 4);

  EXPECT_EQ(vec1[0], vec1.x());
  EXPECT_EQ(vec1[1], vec1.y());
  EXPECT_EQ(vec1[2], vec1.z());
  EXPECT_EQ(vec1[3], vec1.w());
}
