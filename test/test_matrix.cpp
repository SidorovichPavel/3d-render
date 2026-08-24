#include <gtest/gtest.h>

import math;
import math.simd;

using math::Matrix;

TEST(MatrixTest, Constructor) {
  using mat4i = const Matrix<int, 4, 4>;

  EXPECT_EQ(mat4i(), mat4i());

  mat4i mat1;
  mat4i mat2{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  EXPECT_EQ(mat1, mat2);

  mat4i mat3(1);
  mat4i mat4{1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
  EXPECT_EQ(mat3, mat4);

  // EXPECT_EQ(mat8, mat6);
  //
  // mat4i mat7{14, 6, 5, 0, 3, 1, 7, 0, 10, 8, 12, 0, 0, 0, 0, 1};
  //
  // EXPECT_EQ(RankUp(mat8), mat7);
}

TEST(MatrixTest, RankDown) {
  using mat4i = const Matrix<int, 4, 4>;
  using mat3i = const Matrix<int, 3, 3>;

  const mat4i mat1{14, 6, 5, 9, 3, 1, 7, 11, 10, 8, 12, 4, 1, 2, 6, 19};

  const mat3i mat2{14, 6, 5, 3, 1, 7, 10, 8, 12};

  using math::RankDown;

  const auto mat3 = RankDown(mat1);

  EXPECT_EQ(std::ranges::distance(mat3), 9);

  EXPECT_EQ(mat3, mat2);
}

TEST(MatrixTest, RankUp) {
  using mat4i = const Matrix<int, 4, 4>;
  using mat3i = const Matrix<int, 3, 3>;

  const mat4i mat1{14, 6, 5, 0, 3, 1, 7, 0, 10, 8, 12, 0, 0, 0, 0, 1};

  const mat3i mat2{14, 6, 5, 3, 1, 7, 10, 8, 12};

  using math::RankUp;

  const auto mat3 = RankUp(mat2);

  EXPECT_EQ(std::ranges::distance(mat3), 16);

  EXPECT_EQ(mat3, mat1);
}

TEST(MatrixTest, Assighn) {
  using mat4i = Matrix<int, 4, 4>;
  const mat4i mat1(1);
  mat4i mat2;
  EXPECT_NE(mat1, mat2);
  mat2 = mat1;
  EXPECT_EQ(mat1, mat2);
}

TEST(MatrixTest, Multiplication4x4) {
  using mat4i = const Matrix<int, 4, 4>;

  mat4i mat1(1);
  mat4i mat2{14, 6, 5, 9, 3, 1, 7, 11, 10, 8, 12, 4, 1, 2, 6, 19};
  mat4i mat3 = mat1 * mat2;
  EXPECT_EQ(mat3, mat2);

  // clang-format off
  mat4i mat4{
    14, 6, 5,  9,
    3,  1, 7,  11,
    10, 8, 12, 4,
    1,  2, 6,  19
  };
  mat4i mat5{
    12, 6,  9,  4,
    7,  2,  11, 10,
    15, 2,  9,  4,
    1,  11, 9,  3
  };
  mat4i mat6{
    294, 205, 318, 163,
    159, 155, 200, 83,
    360, 144, 322, 180,
    135, 231, 256, 105,
  };
  // clang-format on

  EXPECT_EQ(mat4 * mat5, mat6);

  math::simd::mat4 mat7(mat4), mat8(mat5), mat9(mat6);
  EXPECT_EQ(mat7 * mat8, mat9);
  math::simd::mat4i mat10(mat4), mat11(mat5), mat12(mat6);
  EXPECT_EQ(mat10 * mat11, mat12);
}
