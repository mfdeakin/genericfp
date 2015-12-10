
#include <stdio.h>
#include <gtest/gtest.h>
#include <fenv.h>
#include <math.h>
#include <time.h>
#include <limits.h>
#include "genericfp.hpp"
#include "horner.hpp"

using namespace GenericFP;

TEST(StructTest, TestStructProperties) {
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

TEST(FPExpBitsTest, FPExpBitsTest) {
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

TEST(FPNaNInfTest, NaNInfTest) {
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

TEST(FPGetBitTest, GetBitTest) {
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

TEST(compilertest, compilersetbitfield) {
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

TEST(FPRoundTest, DefaultRoundNearestTest) {
  fesetround(FE_TONEAREST);
  double testValues[] = {
      1.0, 0.0,
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
      /* Test of 1*2^128;
       * should be inf */
      340282366920938463463374607431768211456.0,
      /* Test of 0x800001*2^-23*2^-127;
       * should be exact */
      5.87747245476066970225221814797602003405139342399141e-39,
      /* Test of 0x800001*2^-23*2^-128;
       * should be exact */
      2.93873622738033485112610907398801001702569671199570e-39,
      /* Test of 0x800001*2^-23*2^-129;
       * should be exact */
      1.46936811369016742556305453699400500851284835599785e-39,
      /* Test of 1*2^-23*2^-126;
       * should be exact */
      1.40129846432481707092372958328991613128026194187652e-45,
      /* Test of 1*2^-23*2^-127;
       * should be +0 */
      7.00649232162408535461864791644958065640130970938258e-46,
      /* Test of 1*2^-23*2^-127;
       * should be -0 */
      -7.00649232162408535461864791644958065640130970938258e-46};
  const unsigned numTests =
      sizeof(testValues) / sizeof(testValues[0]);
  for(unsigned i = 0; i < numTests; i++) {
    union {
      fp64 s;
      double f;
    } exact;
    exact.f = testValues[i];

    union {
      fp64 s;
      double f;
    } rounded64;
    rounded64.s = gfRoundNearest<fp64, fp64>(exact.s);
    union {
      fp64 s;
      double f;
    } expTest64;
    expTest64.f = (double)exact.f;
    EXPECT_EQ(expTest64.f, rounded64.f)
        << "ACTUAL Exp: " << rounded64.s.exponent
        << " Sign: " << rounded64.s.sign
        << " Mantissa: " << rounded64.s.mantissa
        << " EXPECTED Exp: " << expTest64.s.exponent
        << " Sign: " << expTest64.s.sign
        << " Mantissa: " << expTest64.s.mantissa << "\n";

    union {
      fp32 s;
      float f;
    } rounded32;
    rounded32.s = gfRoundNearest<fp32, fp64>(exact.s);
    union {
      fp32 s;
      float f;
    } expTest32;
    expTest32.f = (float)exact.f;
    EXPECT_EQ(expTest32.f, rounded32.f)
        << "ACTUAL Exp: " << rounded32.s.exponent
        << " Sign: " << rounded32.s.sign
        << " Mantissa: " << rounded32.s.mantissa
        << " EXPECTED Exp: " << expTest32.s.exponent
        << " Sign: " << expTest32.s.sign
        << " Mantissa: " << expTest32.s.mantissa << "\n";
  }
}

float multiplyTriple(float a, float b, float c) {
  /* Multiplies triples with correct rounding of the final
   * result
   * Implementation without correct rounding costs 2 FLOPS
   */
  float m1 = a * b;
  float r1 = fmaf(a, b, -m1);
  float m2 = r1 * c;
  float r2 = fmaf(r1, c, -m2);
  float result = fmaf(m1, c, m2);
  float err = result - m2;
  return result;
}

float multiplyQuadruple(float a, float b, float c,
                        float d) {
  float m1 = a * b;
  float r1 = fmaf(a, b, -m1);
  float m2 = c * d;
  float r2 = fma(c, d, -m2);
}

float famf(float x, float y, float z) {
  /* Performs (x + y) * z with correct rounding */
  float g = fabs(x) > fabs(y) ? x : y;
  float l = fabs(x) > fabs(y) ? y : x;
  float sum = g + l;
  float residual = (sum - g) - l;
  float prod = residual * z;
  float result = fmaf(sum, z, -prod);
  return result;
}

TEST(tripleMultTest, tmt) {
  union {
    fp32 s;
    float f;
  } x, y, z, correct, attempt, triple;
  x.f = 1.0;
  y.f = 1.0;
  z.f = 1.0;
  for(x.s.mantissa = 1;; x.s.mantissa++) {
    for(y.s.mantissa = 1;; y.s.mantissa++) {
      for(z.s.mantissa = 1;; z.s.mantissa++) {
        double a = x.f, b = y.f, c = z.f;
        correct.f = (float)(a * b * c);
        triple.f = multiplyTriple(x.f, y.f, z.f);
        if(triple.f != correct.f) {
          const unsigned long denorm = 1 << fp32::pBits;
          unsigned long realC = correct.s.mantissa + denorm;
          unsigned long realA = triple.s.mantissa + denorm;
          unsigned long realX = x.s.mantissa + denorm;
          unsigned long realY = y.s.mantissa + denorm;
          unsigned long realZ = z.s.mantissa + denorm;
          unsigned long tripleR =
              triple.s.mantissa + denorm;
          printf(
              "Multiply Triples Test: %6x, %6x, %6x, *, "
              "%6x, *, %6x\n",
              realC, realA, realX, realY, realZ);
        }

        correct.f = (float)((a + b) * c);
        triple.f = famf(a, b, c);
        if(triple.f != correct.f) {
          const unsigned long denorm = 1 << fp32::pBits;
          unsigned long realC = correct.s.mantissa + denorm;
          unsigned long realA = triple.s.mantissa + denorm;
          unsigned long realX = x.s.mantissa + denorm;
          unsigned long realY = y.s.mantissa + denorm;
          unsigned long realZ = z.s.mantissa + denorm;
          unsigned long tripleR =
              triple.s.mantissa + denorm;
          printf(
              "FAM Test: %6x, %6x, %6x, *, %6x, *, %6x\n",
              realC, realA, realX, realY, realZ);
        }

        if(gfManAllSet(z.s)) break;
      }
      if(gfManAllSet(y.s)) break;
    }
    if(gfManAllSet(x.s)) break;
  }
}

TEST(hornersTest, horners) {}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  srand(time(NULL));
  return RUN_ALL_TESTS();
}
