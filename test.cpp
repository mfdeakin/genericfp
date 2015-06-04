
#include <stdio.h>
#include <gtest/gtest.h>
#include <fenv.h>
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

TEST(compilertest, compilersetbitfield)
{
	fp32 test;
	test.exponent = 0;
	test.sign = 0;
	test.mantissa = 0;
	test.mantissa = ~test.mantissa;
	ASSERT_EQ(0, test.exponent);
	ASSERT_EQ(0, test.sign);
	for(int i = 0; i < test.pBits; i++) {
		ASSERT_EQ(1, gfGetMantissaBit<fp32>(test, i));
	}
	test.mantissa = 0;
	test.exponent = ~test.exponent;
	ASSERT_EQ(0, test.mantissa);
	ASSERT_EQ(0, test.sign);
	for(int i = 0; i < test.eBits; i++) {
		ASSERT_EQ(1, gfGetExponentBit<fp32>(test, i));
	}
  
  union {
    fp64 s;
    double f;
  } dbltest;
  dbltest.f = 1.0;
  ASSERT_EQ(0, dbltest.s.mantissa);
  ASSERT_EQ(0x3ff, dbltest.s.exponent);
  ASSERT_EQ(0, dbltest.s.sign);
}

TEST(FPRoundTest, RoundNearestTest)
{
	fesetround(FE_TONEAREST);
	double testValues[] = {1.0,
												 /* Test of 1+2^-23;
													* should be exact rounding */
												 1.00000011920928955078125,
												 /* Test of 1+2^-24;
													* should be 1.0 */
												 1.000000059604644775390625,
												 /* Test of 1+(2+1)*2^-24;
													* should be 1+2^-23 */
												 1.000000178813934326171875,
												 /* Test of 1+(2+1)*2^-25;
													* should be 1+2^-23 */
												 1.0000000894069671630859375,
												 /* Test of 1+(2-1)*2^-25;
													* should be 1.0 */
												 1.0000000298023223876953125,
	};
	const unsigned numTests = sizeof(testValues) /
		sizeof(testValues[0]);
	for(unsigned i = 0; i < numTests; i++) {
		union {
			fp64 s;
			double f;
		} exact;
		exact.f = testValues[i];
		float correct = (float)exact.f;
		
		union {
			fp32 s;
			float f;
		} rounded;
		rounded.s = gfRoundNearest<fp32, fp64>(exact.s);
    union {
      fp32 s;
      float f;
    } expTest;
    expTest.f = correct;
		EXPECT_EQ(correct, rounded.f);
	}
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  srand(time(NULL));
  return RUN_ALL_TESTS();
}
