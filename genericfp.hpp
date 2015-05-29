
#ifndef _GENFP_H_
#define _GENFP_H_

#include <assert.h>

template<unsigned e, unsigned p> struct fp {
  unsigned long mantissa : p;
  unsigned long exponent : e;
  unsigned sign : 1;
  static const unsigned eBits = e, pBits = p;
} __attribute__((packed));

const unsigned gf16ExpBits = 4;
const unsigned gf16ManBits = 11;
const unsigned gf32ExpBits = 8;
const unsigned gf32ManBits = 23;
const unsigned gf64ExpBits = 10;
const unsigned gf64ManBits = 53;

typedef fp<gf16ExpBits, gf16ManBits> fp16;
typedef fp<gf32ExpBits, gf32ManBits> fp32;
typedef fp<gf64ExpBits, gf64ManBits> fp64;

template <typename T> bool gfExpAllSet(T f);
template <typename T> bool gfIsNaN(T f);
template <typename T> bool gfIsInf(T f);

template <typename T> bool getMantissaBit(T f, unsigned bitPos);
template <typename T> bool getExponentBit(T f, unsigned bitPos);
template <typename TDest, typename TSrc> TDest gfRound(TSrc f);

template <typename T> bool gfExpAllSet(T f)
{
  unsigned long long cmp = (1 << f.eBits) - 1;
  return cmp == f.exponent;
}

template <typename T> bool gfIsNaN(T f)
{
  return (gfExpAllSet<T>(f) == true) &&
    (f.mantissa != 0);
}

template <typename T> bool gfIsInf(T f)
{
  return (gfExpAllSet<T>(f) == true) &&
    (f.mantissa == 0);
}

template <typename T> bool gfGetMantissaBit(T f, unsigned bitPos)
{
  assert(bitPos < f.pBits);
  unsigned long selector = 1 << bitPos;
  unsigned long bit = f.mantissa & selector;
  bit >>= bitPos;
  return bit;
}

template <typename T> bool gfGetExponentBit(T f, unsigned bitPos)
{
  assert(bitPos < f.eBits);
  unsigned long selector = 1 << bitPos;
  unsigned long bit = f.exponent & selector;
  bit >>= bitPos;
  return bit;
}

template <typename TDest, typename TSrc> TDest gfRoundNearest(TSrc f)
{
  TDest dest;
}

#endif
