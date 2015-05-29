
#include <stdio.h>
#include <gtest/gtest.h>
#include <math.h>
#include <time.h>
#include "genericfp.hpp"

TEST(StructTest, TestStructProperties)
{
  EXPECT_EQ(sizeof(fp32), sizeof(float));
  EXPECT_EQ(sizeof(fp64), sizeof(double));
  union {
    fp32 s;
    float f;
  } ft;
  ft.f = 1.0f;
  EXPECT_EQ(ft.s.sign, 0);
  unsigned expVal = 0x7f;
  unsigned manVal = 0;
  EXPECT_EQ(expVal, ft.s.exponent);
  EXPECT_EQ(manVal, ft.s.mantissa);
  ft.f *= 2;
  EXPECT_EQ(expVal + 1, ft.s.exponent);
  EXPECT_EQ(manVal, ft.s.mantissa);
  ft.f = -1.0f;
  EXPECT_EQ(1, ft.s.sign);
  EXPECT_EQ(expVal, ft.s.exponent);
  ft.f *= 2;
  EXPECT_EQ(expVal + 1, ft.s.exponent);
  EXPECT_EQ(manVal, ft.s.mantissa);

  ft.f = 1.5;
  manVal = 0x400000;
  EXPECT_EQ(expVal, ft.s.exponent);
  EXPECT_EQ(manVal, ft.s.mantissa);
  ft.f *= 2;
  EXPECT_EQ(expVal + 1, ft.s.exponent);
  EXPECT_EQ(manVal, ft.s.mantissa);

  ft.f = 1.25;
  manVal = 0x200000;
  EXPECT_EQ(expVal, ft.s.exponent);
  EXPECT_EQ(manVal, ft.s.mantissa);
  ft.f *= 2;
  EXPECT_EQ(expVal + 1, ft.s.exponent);
  EXPECT_EQ(manVal, ft.s.mantissa);
}

TEST(FPExpBitsTest, FPExpBitsTest)
{
  union {
    fp16 s;
    unsigned short v;
  } bst;
  for(bst.v = 0; bst.v < 0x7800; bst.v++) {
    ASSERT_EQ(false, gfExpAllSet<fp16>(bst.s));
  }
  for(; bst.v < 0x8000; bst.v++) {
    ASSERT_EQ(true, gfExpAllSet<fp16>(bst.s));
  }
  for(; bst.v < 0xf800; bst.v++) {
    ASSERT_EQ(false, gfExpAllSet<fp16>(bst.s));
  }
  for(; bst.v < 0xffff; bst.v++) {
    ASSERT_EQ(true, gfExpAllSet<fp16>(bst.s));
  }
  ASSERT_EQ(true, gfExpAllSet<fp16>(bst.s));
  
  // union {
  //   fp32 s;
  //   unsigned v;
  // } bt;
  // for (bt.v = 0; bt.v < 0x7f800000; bt.v++) {
  //   ASSERT_EQ(false, gfExpAllSet<fp32>(bt.s));
  // }
  // for(; bt.v < 0x80000000; bt.v++) {
  //   ASSERT_EQ(true, gfExpAllSet<fp32>(bt.s));
  // }
  // for(; bt.v < 0xff800000; bt.v++) {
  //   ASSERT_EQ(false, gfExpAllSet<fp32>(bt.s));
  // }
  // for(; bt.v < 0xffffffff; bt.v++) {
  //   ASSERT_EQ(true, gfExpAllSet<fp32>(bt.s));
  // }
  // ASSERT_EQ(true, gfExpAllSet<fp32>(bt.s));
}

TEST(FPNaNInfTest, NaNInfTest)
{
  union {
    fp32 s;
    float f;
    unsigned bits;
  } ft;
  // for(ft.bits = 0; ft.bits != 0xffffffff; ft.bits++) {
  //   bool comp = (isinf(ft.f) != 0);
  //   ASSERT_EQ(comp, gfIsInf<fp32>(ft.s));
  //   comp = (isnan(ft.f) != 0);
  //   ASSERT_EQ(comp, gfIsNaN<fp32>(ft.s));
  // }
  ASSERT_EQ(isinf(ft.f), gfIsInf<fp32>(ft.s));
}

TEST(FPGetBitTest, GetBitTest)
{
  fp16 s = {0, 0, 0};
  for(unsigned i = 0; i < s.pBits; i++) {
    unsigned compare = 1 << i;
    s.mantissa = compare;
    for(int j = 0; j < s.pBits; j++) {
      unsigned expected = compare & 1;
      ASSERT_EQ(expected, gfGetMantissaBit<fp16>(s, j));
      compare >>= 1;
    }
  }
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  srand(time(NULL));
  return RUN_ALL_TESTS();
}
