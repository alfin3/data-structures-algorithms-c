/**
   utilities-rand-uint64.h

   Declarations of accessible randomness utility functions.

   A randomized approach is used to generate random numbers in a given range
   by exponentially decreasing the probability of not finding a number
   bounded by 0.5^n under the assumption of generator uniformity, where n is
   the number of generated candidate numbers. n is less or equal to 2 in
   expectation.

   The implementation is based on random() that returns a random number from
   0 to RAND_MAX, where RAND_MAX is 2^31 - 1, with a large period of approx.
   16 * (2^31 - 1). The provided functions are seeded by seeding random() 
   outside the provided functions. The implementation is not suitable for
   cryptographic use. (https://man7.org/linux/man-pages/man3/random.3.html)

*/

#ifndef UTILITIES_RAND_UINT64_H  
#define UTILITIES_RAND_UINT64_H

#include <stdint.h>

/**
   Returns a generator-uniform random uint64_t in [0 , n).
*/
uint64_t random_range_uint64(uint64_t n);

/**
   Returns a generator-uniform random uint64_t. 
*/
uint64_t random_uint64();

#endif
