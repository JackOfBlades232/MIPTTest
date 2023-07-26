#pragma once
#include <stdint.h>

typedef float     f32;
typedef double    f64;
typedef uint8_t   u8; 
typedef uint16_t  u16; 
typedef uint32_t  u32;
typedef uint64_t  u64;
typedef int8_t    s8;
typedef int16_t   s16;
typedef int32_t   s32;
typedef int64_t   s64;

#define MIN(_a, _b) (_a < _b ? _a : _b)
#define MAX(_a, _b) (_a > _b ? _a : _b)
#define CLIP(_x, _min, _max) (MAX(_min, MIN(_max, _x)))

#define EPSILON 0.000001

#define ABS(_a) (_a >= 0 ? _a : -_a)
#define FZERO(_a) (_a < EPSILON && _a > -EPSILON)
#define FEQ(_a, _b) FZERO(_a - _b)
#define FLESS(_a, _b) (_b - _a >= EPSILON)
#define FLEQ(_a, _b) (_b - _a > -EPSILON)

template <class T>
inline void swap_generic(T *a, T *b)
{
    T tmp = *a;
    *a = *b;
    *b = tmp;
}

inline void swapf(f32 *a, f32 *b) { swap_generic<f32>(a, b); }
