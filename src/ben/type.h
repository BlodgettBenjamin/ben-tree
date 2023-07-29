#pragma once
#define _BEN_TYPE

#ifdef _STDINT
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t   i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;

#define I8_MIN  INT8_MIN  
#define I16_MIN INT16_MIN 
#define I32_MIN INT32_MIN 
#define I64_MIN INT64_MIN 
#define I8_MAX  INT8_MAX  
#define I16_MAX INT16_MAX 
#define I32_MAX INT32_MAX 
#define I64_MAX INT64_MAX 
#define U8_MAX  UINT8_MAX 
#define U16_MAX UINT16_MAX
#define U32_MAX UINT32_MAX
#define U64_MAX UINT64_MAX

#define I8_SIZE        256ui64
#define I16_SIZE       65536ui64
#define I32_SIZE       4294967296ui64
#define I64_SIZE       18446744073709552000ui64
#define U8_SIZE        256ui64
#define U16_SIZE       65536ui64
#define U32_SIZE       4294967296ui64
#define U64_SIZE       18446744073709552000ui64
#else
typedef signed char        i8;
typedef short              i16;
typedef int                i32;
typedef long long          i64;
typedef unsigned char      u8;
typedef unsigned short     u16;
typedef unsigned int       u32;
typedef unsigned long long u64;

#define I8_MIN        (-127i8 - 1)
#define I16_MIN       (-32767i16 - 1)
#define I32_MIN       (-2147483647i32 - 1)
#define I64_MIN       (-9223372036854775807i64 - 1)
#define I8_MAX        127i8
#define I16_MAX       32767i16
#define I32_MAX       2147483647i32
#define I64_MAX       9223372036854775807i64
#define U8_MAX        0xffui8
#define U16_MAX       0xffffui16
#define U32_MAX       0xffffffffui32
#define U64_MAX       0xffffffffffffffffui64

#define I8_SIZE        256ui64
#define I16_SIZE       65536ui64
#define I32_SIZE       4294967296ui64
#define I64_SIZE       18446744073709552000ui64
#define U8_SIZE        256ui64
#define U16_SIZE       65536ui64
#define U32_SIZE       4294967296ui64
#define U64_SIZE       18446744073709552000ui64
#endif