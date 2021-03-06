/**
   utilities-mod.c

   Utility functions in modular arithmetic. The provided implementations
   assume that CHAR_BIT * sizeof(size_t) is even.
*/

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "utilities-mod.h"

static const size_t BYTE_BIT_COUNT = CHAR_BIT;
static const size_t FULL_BIT_COUNT = CHAR_BIT * sizeof(size_t);
static const size_t HALF_BIT_COUNT = CHAR_BIT * sizeof(size_t) / 2;
static const size_t LOW_MASK = (size_t)-1 >> (CHAR_BIT * sizeof(size_t) / 2);

/**
   Computes overflow-safe mod n of the kth power in O(logk) time,
   based on the binary representation of k and inductively applying the
   following relations:
   if a1 ≡ b1 (mod n) and a2 ≡ b2 (mod n) then 
   a1 a2 ≡ b1 b2 (mod n), and a1 + a2 ≡ b1 + b2 (mod n).
*/
size_t pow_mod(size_t a, size_t k, size_t n){
  size_t k_shift = k;
  size_t ret;
  if (n == 1) return 0;
  ret = 1;
  while (k_shift){
    if (k_shift & 1){
      ret = mul_mod(ret, a, n); /* update for each set bit */
    }
    a = mul_mod(a, a, n); /* repetitive squaring between updates */
    k_shift >>= 1;
  }
  return ret;
}

/**
   Computes overflow-safe (a * b) mod n, by applying the following relation:
   if a1 ≡ b1 (mod n) and a2 ≡ b2 (mod n) then a1 + a2 ≡ b1 + b2 (mod n).
*/
size_t mul_mod(size_t a, size_t b, size_t n){
  size_t al, bl, ah, bh, ah_bh;
  size_t ret;
  size_t i;
  /* comparisons for speed up */
  if (n == 1) return 0;
  if (a == 0 || b == 0) return 0;
  if (a < pow_two(HALF_BIT_COUNT) && b < pow_two(HALF_BIT_COUNT)){
    return (a * b) % n;
  }
  al = a & LOW_MASK;
  bl = b & LOW_MASK;
  ah = a >> HALF_BIT_COUNT;
  bh = b >> HALF_BIT_COUNT;
  ah_bh = ah * bh;
  for (i = 0; i < HALF_BIT_COUNT; i++){
    ah_bh = sum_mod(ah_bh, ah_bh, n);
  }
  ret = sum_mod(ah_bh, ah * bl, n);
  ret = sum_mod(ret, al * bh, n);
  for (i = 0; i < HALF_BIT_COUNT; i++){
    ret = sum_mod(ret, ret, n);
  }
  ret = sum_mod(ret, al * bl, n);
  return ret;
}

/**
   Computes overflow-safe (a + b) mod n, by applying the following relation:
   if a1 ≡ b1 (mod n) and a2 ≡ b2 (mod n) then a1 + a2 ≡ b1 + b2 (mod n).
*/
size_t sum_mod(size_t a, size_t b, size_t n){
  size_t rem;
  if (n == 1) return 0;
  if (a == 0) return b % n;
  if (b == 0) return a % n;
  if (a >= n) a = a % n;
  if (b >= n) b = b % n;
  /* a, b < n, can subtract at most one n from a + b */
  rem = n - a; /* >= 1 */
  if (rem <= b){
    return b - rem;
  }else{
    return a + b;
  }
}

/**
   Computes mod n of a memory block, treating each byte of the block in
   the little-endian order and inductively applying the following relations:
   if a1 ≡ b1 (mod n) and a2 ≡ b2 (mod n) then 
   a1 a2 ≡ b1 b2 (mod n), and a1 + a2 ≡ b1 + b2 (mod n).
   Does not require a little-endian machine.
*/
size_t mem_mod(const void *s, size_t size, size_t n){
  unsigned char *ptr = (unsigned char *)s;
  size_t val, prod;
  size_t ptwo = 1, ptwo_inc;
  size_t ret = 0;
  size_t i;
  if (n == 1) return 0;
  ptwo_inc = mul_mod(pow_two(BYTE_BIT_COUNT - 1), 2, n);
  for (i = 0; i < size; i++){
    val = *ptr;
    /* comparison for speed up across a large memory block */
    if (val >= n) val = val % n;
    prod = mul_mod(ptwo, val, n);
    ret = sum_mod(ret, prod, n);
    ptwo = mul_mod(ptwo, ptwo_inc, n);
    ptr++;
  }
  return ret;
}

/**
   Computes mod n of a memory block, treating the block in sizeof(size_t)-
   byte increments in the little-endian order and inductively applying the
   following relations:
   if a1 ≡ b1 (mod n) and a2 ≡ b2 (mod n) then 
   a1 a2 ≡ b1 b2 (mod n), and a1 + a2 ≡ b1 + b2 (mod n).
   Given a little-endian machine, the return value is equal to the return
   value of mem_mod.
*/
size_t fast_mem_mod(const void *s, size_t size, size_t n){
  size_t *ptr = (size_t *)s;
  size_t step_size = sizeof(size_t);
  size_t res_size = 0;
  size_t val, prod;
  size_t ptwo = 1, ptwo_inc = 1;
  size_t ret = 0;
  size_t i;
  if (n == 1) return 0;
  if (size != step_size){
    res_size = size % step_size;
  }
  if (size > step_size){
    ptwo_inc = mul_mod(pow_two(FULL_BIT_COUNT - 1), 2, n);
  } 
  for (i = 0; i < size - res_size; i += step_size){
    val = *ptr;
    /* comparison for speed across a large memory block */
    if (val >= n) val = val % n;
    prod = mul_mod(ptwo, val, n);
    ret = sum_mod(ret, prod, n);
    ptwo = mul_mod(ptwo, ptwo_inc, n);
    ptr++;
  }
  ptr = (size_t *)((char *)s + size - res_size);
  prod = mul_mod(ptwo, mem_mod(ptr, res_size, n), n);
  ret = sum_mod(ret, prod, n);
  return ret;
}

/**
   Computes mod 2^{8 * sizeof(size_t)} of a product in an overflow-safe
   manner, without using wrapping around. The explicitly treated overlap
   does not result in a notable speed cost in performance tests at the -O3
   optimization level and is preferred over the wrap around product.
*/
size_t mul_mod_pow_two(size_t a, size_t b){
  size_t al, bl, al_bl;
  size_t overlap;
  al = a & LOW_MASK;
  bl = b & LOW_MASK;
  al_bl = al * bl;
  overlap = ((bl * (a >> HALF_BIT_COUNT) & LOW_MASK) +
	     (al * (b >> HALF_BIT_COUNT) & LOW_MASK) +
	     (al_bl >> HALF_BIT_COUNT));
  return (overlap << HALF_BIT_COUNT) + (al_bl & LOW_MASK);
}

/**
   Multiplies two numbers in an overflow-safe manner and copy the high and low
   bits of the product into preallocated blocks pointed to by h and l.
*/
void mul_ext(size_t a, size_t b, size_t *h, size_t *l){
  size_t al, bl, ah, bh, al_bl, al_bh;
  size_t overlap;
  al = a & LOW_MASK;
  bl = b & LOW_MASK;
  ah = a >> HALF_BIT_COUNT;
  bh = b >> HALF_BIT_COUNT;
  al_bl = al * bl;
  al_bh = al * bh;
  overlap = ((bl * ah & LOW_MASK) +
	     (al_bh & LOW_MASK) +
	     (al_bl >> HALF_BIT_COUNT));
  *h = ((overlap >> HALF_BIT_COUNT) +
	ah * bh +
	(ah * bl >> HALF_BIT_COUNT) +
	(al_bh >> HALF_BIT_COUNT));
  *l = (overlap << HALF_BIT_COUNT) + (al_bl & LOW_MASK);
}

/**
   Represents n as u * 2^k, where u is odd.
*/
void represent_uint(size_t n, size_t *k, size_t *u){
  size_t c = 0;
  size_t shift_n = n;
  while (shift_n){
    c++;
    shift_n <<= 1;
  }
  *k = FULL_BIT_COUNT - c;
  *u = n >> *k;
}

/**
   Returns the kth power of 2, where 0 <= k < 8 * sizeof(size_t).
*/
size_t pow_two(size_t k){
  return (size_t)1 << k;
} 
