#include <math.h>
#include <stdbool.h>

#include "prime.h"

bool is_prime(const unsigned int x) {
  if (x < 2)
    return false;

  if (x == 2 || x == 3)
    return true;

  if (x % 2 == 0)
    return false;

  for (unsigned int i = 3; i <= floor(sqrt((double)x)); i += 2) {
    if (x % i == 0) {
      return false;
    }
  }

  return true;
}

/*
 * Return the next prime after x, or x if x is prime
 */
unsigned int next_prime(unsigned int x) {
  while (!is_prime(x)) {
    x++;
  }
  return x;
}
