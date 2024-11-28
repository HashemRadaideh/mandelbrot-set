#include "complex.h"

inline Complex compMul(const Complex left, const Complex right) {
  return (Complex){.real = left.real * right.real - left.imag * right.imag,
                   .imag = left.real * right.imag + left.imag * right.real};
}

inline Complex compAdd(const Complex left, const Complex right) {
  return (Complex){.real = left.real + right.real,
                   .imag = left.imag + right.imag};
}
