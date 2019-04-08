#include "gtest/gtest.h"
#include "gtest/gtest_prod.h"

#include "lib_version.h"
#include "matrix.h"

#define _TEST 1



TEST(version, test1)
{
      ASSERT_TRUE(version() > 0);
}

TEST(matrix, create)
{
      roro_lib::matrix<int, -1> matrix;
      ASSERT_TRUE(matrix.size() == 0);
}

TEST(matrix, default_value)
{
      roro_lib::matrix<int, -1> matrix;
      ASSERT_TRUE(matrix[0][0] == -1);
      ASSERT_TRUE(matrix[34523][52435] == -1);

      auto a = matrix[0][0];
      ASSERT_TRUE(a == -1);

      auto b = matrix[434][5234];
      ASSERT_TRUE(b == -1);

      ASSERT_TRUE(matrix.size() == 0);
}

TEST(matrix, assignment)
{
      roro_lib::matrix<int, -1> matrix;
      matrix[100][100] = 314;
      ASSERT_TRUE(matrix[100][100] == 314);
      ASSERT_TRUE(matrix.size() == 1);

      matrix[12345][54321] = 314;
      ASSERT_TRUE(matrix[12345][54321] == 314);
      ASSERT_TRUE(matrix.size() == 2);
}

TEST(matrix, iterator)
{
      roro_lib::matrix<int, -1> matrix;

      matrix[100][100] = 314;

      for (auto c : matrix)
      {
            auto [row, column, v] = c;

            ASSERT_TRUE(row == 100);
            ASSERT_TRUE(column == 100);
            ASSERT_TRUE(v == 314);
      }      
}
