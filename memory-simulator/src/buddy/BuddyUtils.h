#ifndef MATH_UTILS_H
#define MATH_UTILS_H

#include <cmath>

inline size_t nextPowerOf2(size_t n) {
    if (n == 0) return 1;
    size_t p = 1;
    while (p < n) p <<= 1;
    return p;
}

inline bool isPowerOf2(size_t n) {
    return n && !(n & (n - 1));
}

#endif // MATH_UTILS_H
