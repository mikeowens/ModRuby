#ifndef MODRUBY_STD_TYPES_DECLARE
#define MODRUBY_STD_TYPES_DECLARE

/*
** Integers of known sizes.  These typedefs might change for architectures
** where the sizes vary.  Preprocessor macros are available so that the
** types can be conveniently redefined at compile-type, like this:
**
**         cc '-DUINTPTR_TYPE=long long int' ...
*/

//------------------------------------------------------------------------------
// x86
//------------------------------------------------------------------------------

#include "endian.h"

extern void *SwapEndian(void* Addr, const int Nb);

#if defined(_WIN64)
#define __x86_64__
#endif

namespace modruby
{

#if defined(__i386__)

#ifndef INT8_TYPE
# define INT8_TYPE signed char
#endif

#ifndef UINT8_TYPE
# define UINT8_TYPE unsigned char
#endif

#ifndef INT16_TYPE
# define INT16_TYPE short int
# define INT16_FMT "%hi"
#endif

#ifndef UINT16_TYPE
# define UINT16_TYPE unsigned short int
# define UINT16_FMT "%hu"
#endif

#ifndef INT32_TYPE
# define INT32_TYPE int
# define INT32_FMT "%i"
#endif

#ifndef UINT32_TYPE
# define UINT32_TYPE unsigned int
# define UINT32_FMT "%u"
#endif

#ifndef INT64_TYPE
# define INT64_TYPE long long
# define INT64_FMT "%lli"
#endif

#ifndef UINT64_TYPE
# if defined(_MSC_VER) || defined(__BORLANDC__)
#   define UINT64_TYPE unsigned __int64
# else
#   define UINT64_TYPE unsigned long long int
# endif
#   define UINT64_FMT "%llu"
#endif

#ifndef FLOAT32_TYPE
# define FLOAT32_TYPE float
# define FLOAT32_FMT  "%.6f"
#endif

#ifndef FLOAT64_TYPE
# define FLOAT64_TYPE double
# define FLOAT64_FMT  "%.20g"
#endif

#ifndef FLOAT128_TYPE
# define FLOAT128_TYPE long double
# define FLOAT128_FMT "%.20Lg"
#endif

#ifndef  ADDR_TYPE
# define ADDR_TYPE unsigned int
# define ADDR_FMT "%x"
#endif

#endif // __i386__

//------------------------------------------------------------------------------
// AMD64
//------------------------------------------------------------------------------

#if defined(__x86_64__)

#ifndef INT8_TYPE
# define INT8_TYPE signed char
#endif

#ifndef UINT8_TYPE
# define UINT8_TYPE unsigned char
#endif

#ifndef INT16_TYPE
# define INT16_TYPE short int
# define INT16_FMT "%hi"
#endif

#ifndef UINT16_TYPE
# define UINT16_TYPE unsigned short int
# define UINT16_FMT "%hu"
#endif

#ifndef INT32_TYPE
# define INT32_TYPE int
# define INT32_FMT "%i"
#endif

#ifndef UINT32_TYPE
# define UINT32_TYPE unsigned int
# define UINT32_FMT "%u"
#endif

#ifndef INT64_TYPE
# define INT64_TYPE long
# define INT64_FMT "%li"
#endif

#ifndef UINT64_TYPE
# if defined(_MSC_VER) || defined(__BORLANDC__)
#   define UINT64_TYPE unsigned __int64
# else
#   define UINT64_TYPE unsigned long int
# endif
#   define UINT64_FMT "%lu"
#endif

#ifndef FLOAT32_TYPE
# define FLOAT32_TYPE float
# define FLOAT32_FMT  "%f"
#endif

#ifndef FLOAT64_TYPE
# define FLOAT64_TYPE double
# define FLOAT64_FMT  "%g"
#endif

#ifndef FLOAT128_TYPE
# define FLOAT128_TYPE long double
# define FLOAT128_FMT "%Lg"
#endif

#ifndef  ADDR_TYPE
# define ADDR_TYPE unsigned long int
# define ADDR_FMT "%x"
#endif

#endif // __x86_64__

typedef INT8_TYPE       i8; // 1-byte signed integer
typedef UINT8_TYPE      u8; // 1-byte unsigned integer
typedef INT16_TYPE     i16; // 2-byte signed integer
typedef UINT16_TYPE    u16; // 2-byte unsigned integer
typedef INT32_TYPE     i32; // 4-byte signed integer  
typedef UINT32_TYPE    u32; // 4-byte unsigned integer
typedef INT64_TYPE     i64; // 8-byte signed integer
typedef UINT64_TYPE    u64; // 8-byte unsigned integer
typedef FLOAT32_TYPE   f32; // 4-byte floating point number
typedef FLOAT64_TYPE   f64; // 8-byte floating point number
typedef FLOAT128_TYPE f128; // 16-byte floating point number
typedef ADDR_TYPE     addr; // Platform dependent address

#define SWAP_I16(val)  val = *(i16*) swap_endian((void*)&val, sizeof(i16))
#define SWAP_U16(val)  val = *(u16*) swap_endian((void*)&val, sizeof(u16))
#define SWAP_I32(val)  val = *(i32*) swap_endian((void*)&val, sizeof(i32))
#define SWAP_U32(val)  val = *(u32*) swap_endian((void*)&val, sizeof(u32))
#define SWAP_I64(val)  val = *(i64*) swap_endian((void*)&val, sizeof(i64))
#define SWAP_U64(val)  val = *(u64*) swap_endian((void*)&val, sizeof(u64))
#define SWAP_F32(val)  val = *(f32*) swap_endian((void*)&val, sizeof(f32))
#define SWAP_F64(val)  val = *(f64*) swap_endian((void*)&val, sizeof(f64))
#define SWAP_F128(val) val = *(f128*)swap_endian((void*)&val, sizeof(f128))

} // end namespace modruby;

#endif

