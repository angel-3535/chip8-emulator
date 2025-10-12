#ifndef TYPES_H
#define TYPES_H

#include <stdbool.h>
#include <stdint.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef float f32;
typedef double f64;

typedef struct iv2 {
  i32 x;
  i32 y;
} iv2;

typedef struct uv2 {
  u32 x;
  u32 y;
} uv2;

typedef struct fv2 {
  f32 x;
  f32 y;
} fv2;

typedef struct v2 {
  f64 width;
  f64 height;
} v2;

#endif
