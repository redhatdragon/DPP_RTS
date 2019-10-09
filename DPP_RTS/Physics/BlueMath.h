#pragma once
#include <stdint.h>

typedef uint8_t uint8;
typedef int8_t int8;
typedef uint16_t uint16;
typedef int16_t int16;
typedef uint32_t uint32;
typedef int32_t int32;
typedef uint64_t uint64;
typedef int64_t int64;

struct Point {
    float x, y;
    inline Point operator+(Point other){
        return {x+other.x, y+other.y};
    }
    inline void operator+=(Point other){
        x+=other.x; y+=other.y;
    }
    inline Point operator-(Point other){
        return {x-other.x, y-other.y};
    }
    inline void operator-=(Point other){
        x-=other.x; y-=other.y;
    }
    inline Point operator*(float num){
        return {x*num, y*num};
    }
    inline void operator*=(float num){
        x*=num; y*=num;
    }
    inline Point operator/(float num){
        return {x/num, y/num};
    }
    inline void operator/=(float num){
        x/=num; y/=num;
    }
};

struct Rect {
    float x, y, w, h;
};