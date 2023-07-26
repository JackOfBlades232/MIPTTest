#pragma once
#include "Utils.h"

template <class T>
struct vec2_generic_t {
    T x;
    T y;

    inline vec2_generic_t() : x(0), y(0) {}
    inline vec2_generic_t(T _x, T _y) : x(_x), y(_y) {}
    inline vec2_generic_t(const vec2_generic_t &other) : x(other.x), y(other.y) {}

    inline vec2_generic_t operator +(const vec2_generic_t &other)   { return vec2_generic_t(x + other.x, y + other.y); }
    inline vec2_generic_t operator -(const vec2_generic_t &other)   { return vec2_generic_t(x + other.x, y + other.y); }
    inline vec2_generic_t operator -()                              { return vec2_generic_t(-x, -y); }
    inline vec2_generic_t operator *(T scalar)                      { return vec2_generic_t(x*scalar, y*scalar); }
    inline vec2_generic_t operator /(T scalar)                      { return vec2_generic_t(x/scalar, y/scalar); }
    inline vec2_generic_t &operator +=(const vec2_generic_t &other) { x += other.x, y += other.y; return *this; }
    inline vec2_generic_t &operator -=(const vec2_generic_t &other) { x -= other.x, y -= other.y; return *this; }
    inline vec2_generic_t &operator *=(T scalar)                    { x *= scalar, y *= scalar; return *this; }
    inline vec2_generic_t &operator /=(T scalar)                    { x /= scalar, y /= scalar; return *this; }
};

template <class T> inline vec2_generic_t<T> operator *(T scalar, const vec2_generic_t<T> &v) { return vec2_generic_t<T>(scalar*v.x, scalar*v.y); }
template <class T> inline vec2_generic_t<T> operator /(T scalar, const vec2_generic_t<T> &v) { return vec2_generic_t<T>(scalar/v.x, scalar/v.y); }

typedef vec2_generic_t<s32> vec2i_t;
typedef vec2_generic_t<f32> vec2f_t;
