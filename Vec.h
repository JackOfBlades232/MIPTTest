#pragma once
#include "Utils.h"
#include <math.h>

template <class T>
struct vec2_generic_t {
    T x;
    T y;

    inline vec2_generic_t() : x(0), y(0) {}
    inline vec2_generic_t(T x, T y) : x(x), y(y) {}
    inline vec2_generic_t(const vec2_generic_t &other) : x(other.x), y(other.y) {}

    inline vec2_generic_t operator +(const vec2_generic_t &other) const { return vec2_generic_t(x + other.x, y + other.y); }
    inline vec2_generic_t operator -(const vec2_generic_t &other) const { return vec2_generic_t(x - other.x, y - other.y); }
    inline vec2_generic_t operator -() const                            { return vec2_generic_t(-x, -y); }
    inline vec2_generic_t operator *(T scalar) const                    { return vec2_generic_t(x*scalar, y*scalar); }
    inline vec2_generic_t operator /(T scalar) const                    { return vec2_generic_t(x/scalar, y/scalar); }
    inline vec2_generic_t &operator +=(const vec2_generic_t &other)     { x += other.x, y += other.y; return *this; }
    inline vec2_generic_t &operator -=(const vec2_generic_t &other)     { x -= other.x, y -= other.y; return *this; }
    inline vec2_generic_t &operator *=(T scalar)                        { x *= scalar, y *= scalar; return *this; }
    inline vec2_generic_t &operator /=(T scalar)                        { x /= scalar, y /= scalar; return *this; }

    inline T mag() const                                                { return sqrt(x*x + y*y); }
    inline vec2_generic_t &normalize()                                  { T r = 1/mag(); return (*this *= r); }
    inline vec2_generic_t normalized() const                            { T r = 1/mag(); return (*this * r); }
    inline bool is_zero() const                                         { return FZERO(x) && FZERO(y); }
    inline T dot(const vec2_generic_t &other) const                     { return x*other.x + y*other.y; }
};

template <class T> inline vec2_generic_t<T> operator *(s32 scalar, const vec2_generic_t<T> &v) { return vec2_generic_t<T>(scalar*v.x, scalar*v.y); }
template <class T> inline vec2_generic_t<T> operator /(s32 scalar, const vec2_generic_t<T> &v) { return vec2_generic_t<T>(scalar/v.x, scalar/v.y); }
template <class T> inline vec2_generic_t<T> operator *(f32 scalar, const vec2_generic_t<T> &v) { return vec2_generic_t<T>(scalar*v.x, scalar*v.y); }
template <class T> inline vec2_generic_t<T> operator /(f32 scalar, const vec2_generic_t<T> &v) { return vec2_generic_t<T>(scalar/v.x, scalar/v.y); }

template <class T> inline vec2_generic_t<T> reflect_vec(const vec2_generic_t<T> &v, const vec2_generic_t<T> &axis)
{
    vec2_generic_t<T> proj = v.dot(axis) * axis; 
    return 2*proj - v;
}

typedef vec2_generic_t<s32> vec2i_t;
typedef vec2_generic_t<f32> vec2f_t;
