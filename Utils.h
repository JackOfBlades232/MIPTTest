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
#define SGN(_a) (_a > 0 ? 1 : (_a < 0 ? -1 : 0))
#define ABS(_a) (_a >= 0 ? _a : -_a)

#define EPSILON 0.000001

#define FZERO(_a) (_a < EPSILON && _a > -EPSILON)
#define FEQ(_a, _b) FZERO(_a - _b)
#define FLESS(_a, _b) (_b - _a >= EPSILON)
#define FLEQ(_a, _b) (_b - _a > -EPSILON)

#if defined(USE_ASSERTIONS) && USE_ASSERTIONS == 1

  #include <stdio.h>

  #define _LOG_STRERR_GEN(_func, _errno, _fmt, ...) fprintf(stderr, "[ERR] (%s:%d: errno: %s) " _fmt "\n", \
          __FILE__, __LINE__, (errno == 0 ? "None" : _func(_errno)), ##__VA_ARGS__)
  #define _LOG_STRERR(_func, _fmt, ...) _LOG_STRERR_GEN(_func, errno, _fmt, ##__VA_ARGS__)
  #define _LOG_ERR(_fmt, ...) _LOG_STRERR(strerror, _fmt, ##__VA_ARGS__)
  
  #define ASSERT(_e) if(!(_e)) { fprintf(stderr, "Assertion (" #_e ") failed at %s:%d\n", __FILE__, __LINE__); exit(1); }
  #define ASSERTF(_e, _fmt, ...) if(!(_e)) { fprintf(stderr, _fmt, ##__VA_ARGS__); exit(1); }
  #define ASSERT_ERR(_e) if(!(_e)) { _LOG_ERR("Assertion (" #_e ") failed at %s:%d\n", __FILE__, __LINE__); exit(1); }
  #define ASSERTF_ERR(_e, _fmt, ...) if(!(_e)) { _LOG_ERR(_fmt, ##__VA_ARGS__); exit(1); }
  
  #define __ASSERT_GLUE(_a, _b) _a ## _b
  #define _ASSERT_GLUE(_a, _b) __ASSERT_GLUE(_a, _b)
  #define STATIC_ASSERT(_e) enum { _ASSERT_GLUE(s_assert_fail_, __LINE__) = 1 / (int)(!!(_e)) }

#else

  #define ASSERT(_e)
  #define ASSERTF(_e, _fmt, ...)
  #define ASSERT_ERR(_e)
  #define ASSERTF_ERR(_e, _fmt, ...)
  #define STATIC_ASSERT(_e)

#endif

template <class T>
inline void swap_generic(T *a, T *b)
{
    T tmp = *a;
    *a = *b;
    *b = tmp;
}

inline void swapf(f32 *a, f32 *b) { swap_generic<f32>(a, b); }
