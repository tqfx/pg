/*!
 @file a.h
 @brief algorithm library
*/

#ifndef LIBA_A_H
#define LIBA_A_H

#if defined(A_HAVE_H)
#include A_HAVE_H
#endif /* A_HAVE_H */

/*! @cond */

#if !defined __has_attribute
#define __has_attribute(x) 0
#endif /* __has_attribute */
#if !defined __has_builtin
#define __has_builtin(x) 0
#endif /* __has_builtin */
#if !defined __has_feature
#define __has_feature(x) 0
#endif /* __has_feature */
#if !defined __has_include
#define __has_include(x) 0
#endif /* __has_include */
#if !defined __has_warning
#define __has_warning(x) 0
#endif /* __has_warning */
#if !defined A_HAVE_INLINE
#define A_HAVE_INLINE 1
#elif A_HAVE_INLINE + 0 < 1
#undef A_HAVE_INLINE
#endif /* A_HAVE_INLINE */
#if defined(__GNUC__) || \
    defined(__clang__)
#pragma GCC system_header
#endif /* __GNUC__ */

#if defined(__MINGW32__)
#undef __USE_MINGW_ANSI_STDIO
#define __USE_MINGW_ANSI_STDIO 1
#endif /* __MINGW32__ */

/* https://en.wikipedia.org/wiki/Microsoft_Visual_C++ */
#if defined(_MSC_VER)
#define A_PREREQ_MSVC(maj, min) (_MSC_VER >= (maj * 100 + min))
#else /* !_MSC_VER */
#define A_PREREQ_MSVC(maj, min) 0
#endif /* _MSC_VER */

/* https://gcc.gnu.org/onlinedocs/cpp/Common-Predefined-Macros.html */
#if defined(__GNUC__) && defined(__GNUC_MINOR__)
#define A_PREREQ_GNUC(maj, min) ((__GNUC__ << 16) + __GNUC_MINOR__ >= ((maj) << 16) + (min))
#else /* !__GNUC__ */
#define A_PREREQ_GNUC(maj, min) 0
#endif /* __GNUC__ */

/* https://clang.llvm.org/docs/LanguageExtensions.html */
#if defined(__clang_major__) && defined(__clang_minor__)
#define A_PREREQ_LLVM(maj, min) ((__clang_major__ << 16) + __clang_minor__ >= ((maj) << 16) + (min))
#else /* !__clang__ */
#define A_PREREQ_LLVM(maj, min) 0
#endif /* __clang__ */

#if !defined A_FUNC
#if defined(_MSC_VER)
#define A_FUNC __FUNCTION__
#elif defined(__GNUC__)
#define A_FUNC (__extension__ __PRETTY_FUNCTION__)
#elif defined(__cplusplus) && (__cplusplus > 201100L) || \
    defined(__STDC_VERSION__) && (__STDC_VERSION__ > 199900L)
#define A_FUNC __func__
#else /* !__func__ */
#define A_FUNC __FUNCTION__
#endif /* __func__ */
#endif /* A_FUNC */

#if defined(__STDC_VERSION__) && (__STDC_VERSION__ > 199900L) || \
    defined(__cplusplus) && (__cplusplus > 201100L) || A_PREREQ_MSVC(18, 0)

#if !defined A_HAVE_VARIADIC_MACROS
#define A_HAVE_VARIADIC_MACROS 1
#endif /* A_HAVE_VARIADIC_MACROS */

#if !defined A_HAVE_LONG_LONG_TYPE
#define A_HAVE_LONG_LONG_TYPE 1
#endif /* A_HAVE_LONG_LONG_TYPE */

#endif /* C > 199900 or C++ > 201100 */
#if defined(A_HAVE_VARIADIC_MACROS) && (A_HAVE_VARIADIC_MACROS + 0 < 1)
#undef A_HAVE_VARIADIC_MACROS
#endif /* A_HAVE_VARIADIC_MACROS */
#if defined(A_HAVE_LONG_LONG_TYPE) && (A_HAVE_LONG_LONG_TYPE + 0 < 1)
#undef A_HAVE_LONG_LONG_TYPE
#endif /* A_HAVE_LONG_LONG_TYPE */

#if defined(__cplusplus) && (__cplusplus > 201100L) || \
    defined(__STDC_VERSION__) && (__STDC_VERSION__ > 201100L)

#if !defined A_HAVE_STATIC_ASSERT
#define A_HAVE_STATIC_ASSERT 1
#endif /* A_HAVE_STATIC_ASSERT */

#endif /* C > 201100 or C++ > 201100 */
#if defined(A_HAVE_STATIC_ASSERT) && (A_HAVE_STATIC_ASSERT + 0 < 1)
#undef A_HAVE_STATIC_ASSERT
#endif /* A_HAVE_STATIC_ASSERT */

#if defined(__cplusplus) && (__cplusplus > 201100L)

#if !defined A_HAVE_NULLPTR
#define A_HAVE_NULLPTR 1
#endif /* A_HAVE_NULLPTR */

#endif /*  C++ > 201100 */
#if defined(A_HAVE_NULLPTR) && (A_HAVE_NULLPTR + 0 < 1)
#undef A_HAVE_NULLPTR
#endif /* A_HAVE_NULLPTR */

#if A_PREREQ_GNUC(2, 96) || __has_builtin(__builtin_expect)
#define A_UNLIKELY(x) __builtin_expect(!!(x), 0)
#define A_LIKELY(x) __builtin_expect(!!(x), 1)
#else /* !likely */
#define A_UNLIKELY(x) (!!(x))
#define A_LIKELY(x) (!!(x))
#endif /* likely */

#if defined(_WIN32) || defined(__CYGWIN__)
#define A_DECLSPEC(x) __declspec(x)
#else /* !__declspec */
#define A_DECLSPEC(x)
#endif /* __declspec */

#if defined(__GNUC__) || defined(__clang__)
#define A_ATTRIBUTE(x) __attribute__(x)
#else /* !__attribute__ */
#define A_ATTRIBUTE(x)
#endif /* __attribute__ */

#if __has_builtin(__builtin_assume)
#define A_ASSUME(x) __builtin_assume(x)
#elif A_PREREQ_GNUC(13, 0)
#define A_ASSUME(x) __attribute__((__assume__(x)))
#elif defined(__GNUC__)
#define A_ASSUME(x)                            \
    do {                                       \
        if (!(x)) { __builtin_unreachable(); } \
    } while (0)
#elif defined(_MSC_VER)
#define A_ASSUME(x) __assume(x)
#else /* !assume */
#define A_ASSUME(x) (void)0
#endif /* assume */

/* attribute nonnull */
#if A_PREREQ_GNUC(3, 3) || __has_attribute(__nonnull__)
#define A_NONULL(x) __attribute__((__nonnull__ x))
#else /* !nonnull */
#define A_NONULL(x)
#endif /* nonnull */

/* attribute format */
#if A_PREREQ_GNUC(2, 4) || __has_attribute(__format__)
#define A_FORMAT(_, a, b) __attribute__((__format__(_, a, b)))
#else /* !format */
#define A_FORMAT(_, a, b)
#endif /* format */

/* attribute fallthrough */
#if A_PREREQ_GNUC(7, 0) || __has_attribute(__fallthrough__)
#define A_FALLTHROUGH __attribute__((__fallthrough__))
#else /* !fallthrough */
#define A_FALLTHROUGH (void)0
#endif /* fallthrough */

/* attribute deprecated */
#if A_PREREQ_GNUC(3, 2) || __has_attribute(__deprecated__)
#define A_DEPRECATED __attribute__((__deprecated__))
#elif defined(_WIN32) || defined(__CYGWIN__)
#define A_DEPRECATED __declspec(deprecated)
#else /* !deprecated */
#define A_DEPRECATED
#endif /* deprecated */

/* attribute always inline */
#if A_PREREQ_GNUC(3, 2) || __has_attribute(__always_inline__)
#define A_INLINE __inline __attribute__((__always_inline__))
#elif defined(_MSC_VER)
#define A_INLINE __inline __forceinline
#else /* !_MSC_VER */
#define A_INLINE __inline
#endif /* _MSC_VER */
#if !defined A_INTERN
#define A_INTERN static A_INLINE
#endif /* A_INTERN */

/* attribute visibility */
#if defined(_WIN32) || defined(__CYGWIN__)
#define A_EXPORT __declspec(dllexport)
#define A_IMPORT __declspec(dllimport)
#define A_HIDDEN
#elif A_PREREQ_GNUC(4, 0) || __has_attribute(__visibility__)
#define A_EXPORT __attribute__((__visibility__("default")))
#define A_IMPORT __attribute__((__visibility__("default")))
#define A_HIDDEN __attribute__((__visibility__("hidden")))
#else /* !visibility */
#define A_EXPORT
#define A_IMPORT
#define A_HIDDEN
#endif /* visibility */
#if defined(A_EXPORTS)
#define A_PUBLIC A_EXPORT
#elif defined(A_IMPORTS)
#define A_PUBLIC A_IMPORT
#else /* !A_PUBLIC */
#define A_PUBLIC
#endif /* A_PUBLIC */

#if !defined A_EXTERN
#define A_EXTERN A_PUBLIC extern
#endif /* A_EXTERN */
#if !defined __cplusplus
#define A_EXTERN_C
#define A_EXTERN_C_ENTER
#define A_EXTERN_C_LEAVE
#else /* !__cplusplus */
#define A_EXTERN_C extern "C"
#define A_EXTERN_C_ENTER extern "C" {
#define A_EXTERN_C_LEAVE }
#endif /* __cplusplus */

/*! @endcond */

/* byte order of little endian architecture */
#if defined(__ORDER_LITTLE_ENDIAN__)
#define A_ORDER_LITTLE __ORDER_LITTLE_ENDIAN__
#else /* !__ORDER_LITTLE_ENDIAN__ */
#define A_ORDER_LITTLE 1234
#endif /* __ORDER_LITTLE_ENDIAN__ */
/* byte order of big endian architecture */
#if defined(__ORDER_BIG_ENDIAN__)
#define A_ORDER_BIG __ORDER_BIG_ENDIAN__
#else /* !__ORDER_BIG_ENDIAN__ */
#define A_ORDER_BIG 4321
#endif /* __ORDER_BIG_ENDIAN__ */
/* byte order of architecture */
#if !defined A_BYTE_ORDER
#if defined(__BYTE_ORDER__)
#define A_BYTE_ORDER __BYTE_ORDER__
#endif /* __BYTE_ORDER__ */
#endif /* A_BYTE_ORDER */

/* size of void pointer */
#if !defined A_SIZE_POINTER
#if defined(__SIZEOF_POINTER__)
#define A_SIZE_POINTER __SIZEOF_POINTER__
#elif defined(_WIN64)
#define A_SIZE_POINTER 8
#elif defined(_WIN32)
#define A_SIZE_POINTER 4
#endif /* __SIZEOF_POINTER__ */
#endif /* A_SIZE_POINTER */

#include <stddef.h>
#include <limits.h>
#include <stdint.h>
#include <float.h>

/*!
 @addtogroup liba algorithm library
 @{
*/

/*! assert a build-time dependency, as an expression */
#define A_BUILD_ASSERT(x) (void)(sizeof(char[1 - 2 * !(x)]))
#define A_BUILD_BUG_ON(x) (void)(sizeof(char[1 - 2 * !!(x)]))

#if defined(__cplusplus)
#define a_cast_r(T, x) reinterpret_cast<T>(x)
#define a_cast_d(T, x) dynamic_cast<T>(x)
#define a_cast_s(T, x) static_cast<T>(x)
#define a_cast_c(T, x) const_cast<T>(x)
#else /* !__cplusplus */
#define a_cast_r(T, x) ((T)(x))
#define a_cast_d(T, x) ((T)(x))
#define a_cast_s(T, x) ((T)(x))
#define a_cast_c(T, x) ((T)(x))
#endif /* __cplusplus */
#define A_CAST_3(a, b, c) a##b##c
#define A_CAST_2(a, b) a##b
#define A_CAST_1(a) #a

#if defined(__cplusplus) && defined(A_HAVE_NULLPTR)
#define A_NULL nullptr
#else /* !__cplusplus */
#define A_NULL NULL
#endif /* __cplusplus */
/*! static cast to \ref a_void */
#define a_void_c(_, x) a_cast_s(void, x)
#define a_void_(_, x) a_cast_s(void _, x)
/*! incomplete type or no parameter or no return value */
#define a_void void

#if defined(__cplusplus)
#define A_TRUE true
#define A_FALSE false
#if !defined A_BOOL
#define A_BOOL bool
#endif /* A_BOOL */
#else /* !__cplusplus */
#define A_TRUE 1
#define A_FALSE 0
#if !defined A_BOOL
#define A_BOOL _Bool
#endif /* A_BOOL */
#endif /* __cplusplus */
/*! static cast to \ref a_bool */
#define a_bool_c(x) (!!(x))
/*! type, capable of holding one of the two values: 1 and 0 */
#define a_bool A_BOOL

#define A_INT int
#define A_INT_MIN INT_MIN
#define A_INT_MAX INT_MAX
/*! static cast to \ref a_int */
#define a_int_c(x) a_cast_s(A_INT, x)
#define a_int_(_, x) a_cast_s(A_INT _, x)
/*! signed integer type is guaranteed to be at least 16 bits */
#define a_int A_INT

#define A_UINT unsigned int
#define A_UINT_MAX UINT_MAX
/*! static cast to \ref a_uint */
#define a_uint_c(x) a_cast_s(A_UINT, x)
#define a_uint_(_, x) a_cast_s(A_UINT _, x)
/*! unsigned integer type is guaranteed to be at least 16 bits */
#define a_uint A_UINT

#define A_SHRT short
#define A_SHRT_MIN SHRT_MIN
#define A_SHRT_MAX SHRT_MAX
/*! static cast to \ref a_shrt */
#define a_shrt_c(x) a_cast_s(A_SHRT, x)
#define a_shrt_(_, x) a_cast_s(A_SHRT _, x)
/*! signed integer type is guaranteed to be at least 16 bits */
#define a_shrt A_SHRT

#define A_USHRT unsigned short
#define A_USHRT_MAX USHRT_MAX
/*! static cast to \ref a_ushrt */
#define a_ushrt_c(x) a_cast_s(A_USHRT, x)
#define a_ushrt_(_, x) a_cast_s(A_USHRT _, x)
/*! unsigned integer type is guaranteed to be at least 16 bits */
#define a_ushrt A_USHRT

#define A_LONG long
#define A_LONG_MIN LONG_MIN
#define A_LONG_MAX LONG_MAX
/*! static cast to \ref a_long */
#define a_long_c(x) a_cast_s(A_LONG, x)
#define a_long_(_, x) a_cast_s(A_LONG _, x)
/*! signed integer type is guaranteed to be at least 32 bits */
#define a_long A_LONG

#define A_ULONG unsigned long
#define A_ULONG_MAX ULONG_MAX
/*! static cast to \ref a_ulong */
#define a_ulong_c(x) a_cast_s(A_ULONG, x)
#define a_ulong_(_, x) a_cast_s(A_ULONG _, x)
/*! unsigned integer type is guaranteed to be at least 32 bits */
#define a_ulong A_ULONG

#if defined(A_HAVE_LONG_LONG_TYPE)

#define A_LLONG long long
#define A_LLONG_MIN LLONG_MIN
#define A_LLONG_MAX LLONG_MAX
/*! static cast to \ref a_llong */
#define a_llong_c(x) a_cast_s(A_LLONG, x)
#define a_llong_(_, x) a_cast_s(A_LLONG _, x)
/*! signed integer type is guaranteed to be at least 64 bits */
#define a_llong A_LLONG

#define A_ULLONG unsigned long long
#define A_ULLONG_MAX ULLONG_MAX
/*! static cast to \ref a_ullong */
#define a_ullong_c(x) a_cast_s(A_ULLONG, x)
#define a_ullong_(_, x) a_cast_s(A_ULLONG _, x)
/*! unsigned integer type is guaranteed to be at least 64 bits */
#define a_ullong A_ULLONG

#endif /* A_HAVE_LONG_LONG_TYPE */

#define A_BYTE unsigned char
#define A_BYTE_MAX UCHAR_MAX
/*! static cast to \ref a_byte */
#define a_byte_c(x) a_cast_s(A_BYTE, x)
#define a_byte_(_, x) a_cast_s(A_BYTE _, x)
/*! type for unsigned character representation */
#define a_byte A_BYTE

#define A_C8 char
#define A_C8_MIN CHAR_MIN
#define A_C8_MAX CHAR_MAX
/*! static cast to \ref a_c8 */
#define a_c8_c(x) a_cast_s(A_C8, x)
#define a_c8_(_, x) a_cast_s(A_C8 _, x)
/*! type for character representation */
#define a_c8 A_C8

#if !defined A_I8
#define A_I8 int8_t
#endif /* A_I8 */
#if !defined A_I8_C && defined(INT8_C)
#define A_I8_C(X) INT8_C(X)
#endif /* A_I8_C */
#if !defined A_I8_MIN && defined(INT8_MIN)
#define A_I8_MIN INT8_MIN
#endif /* A_I8_MIN */
#if !defined A_I8_MAX && defined(INT8_MAX)
#define A_I8_MAX INT8_MAX
#endif /* A_I8_MAX */
/*! static cast to \ref a_i8 */
#define a_i8_c(x) a_cast_s(A_I8, x)
#define a_i8_(_, x) a_cast_s(A_I8 _, x)
/*! signed integer type with width of exactly 8 bits */
#define a_i8 A_I8

#if !defined A_U8
#define A_U8 uint8_t
#endif /* A_U8 */
#if !defined A_U8_C && defined(UINT8_C)
#define A_U8_C(X) UINT8_C(X)
#endif /* A_U8_C */
#if !defined A_U8_MAX && defined(UINT8_MAX)
#define A_U8_MAX UINT8_MAX
#endif /* A_U8_MAX */
/*! static cast to \ref a_u8 */
#define a_u8_c(x) a_cast_s(A_U8, x)
#define a_u8_(_, x) a_cast_s(A_U8 _, x)
/*! unsigned integer type with width of exactly 8 bits */
#define a_u8 A_U8

#if !defined A_I16
#define A_I16 int16_t
#endif /* A_I16 */
#if !defined A_I16_C && defined(INT16_C)
#define A_I16_C(X) INT16_C(X)
#endif /* A_I16_C */
#if !defined A_I16_MIN && defined(INT16_MIN)
#define A_I16_MIN INT16_MIN
#endif /* A_I16_MIN */
#if !defined A_I16_MAX && defined(INT16_MAX)
#define A_I16_MAX INT16_MAX
#endif /* A_I16_MAX */
/*! static cast to \ref a_i16 */
#define a_i16_c(x) a_cast_s(A_I16, x)
#define a_i16_(_, x) a_cast_s(A_I16 _, x)
/*! signed integer type with width of exactly 16 bits */
#define a_i16 A_I16

#if !defined A_U16
#define A_U16 uint16_t
#endif /* A_U16 */
#if !defined A_U16_C && defined(UINT16_C)
#define A_U16_C(X) UINT16_C(X)
#endif /* A_U16_C */
#if !defined A_U16_MAX && defined(UINT16_MAX)
#define A_U16_MAX UINT16_MAX
#endif /* A_U16_MAX */
/*! static cast to \ref a_u16 */
#define a_u16_c(x) a_cast_s(A_U16, x)
#define a_u16_(_, x) a_cast_s(A_U16 _, x)
/*! unsigned integer type with width of exactly 16 bits */
#define a_u16 A_U16

#if !defined A_I32
#define A_I32 int32_t
#endif /* A_I32 */
#if !defined A_I32_C && defined(INT32_C)
#define A_I32_C(X) INT32_C(X)
#endif /* A_I32_C */
#if !defined A_I32_MIN && defined(INT32_MIN)
#define A_I32_MIN INT32_MIN
#endif /* A_I32_MIN */
#if !defined A_I32_MAX && defined(INT32_MAX)
#define A_I32_MAX INT32_MAX
#endif /* A_I32_MAX */
/*! static cast to \ref a_i32 */
#define a_i32_c(x) a_cast_s(A_I32, x)
#define a_i32_(_, x) a_cast_s(A_I32 _, x)
/*! signed integer type with width of exactly 32 bits */
#define a_i32 A_I32

#if !defined A_U32
#define A_U32 uint32_t
#endif /* A_U32 */
#if !defined A_U32_C && defined(UINT32_C)
#define A_U32_C(X) UINT32_C(X)
#endif /* A_U32_C */
#if !defined A_U32_MAX && defined(UINT32_MAX)
#define A_U32_MAX UINT32_MAX
#endif /* A_U32_MAX */
/*! static cast to \ref a_u32 */
#define a_u32_c(x) a_cast_s(A_U32, x)
#define a_u32_(_, x) a_cast_s(A_U32 _, x)
/*! unsigned integer type with width of exactly 32 bits */
#define a_u32 A_U32

#if !defined A_I64
#define A_I64 int64_t
#endif /* A_I64 */
#if !defined A_I64_C && defined(INT64_C)
#define A_I64_C(X) INT64_C(X)
#endif /* A_I64_C */
#if !defined A_I64_MIN && defined(INT64_MIN)
#define A_I64_MIN INT64_MIN
#endif /* A_I64_MIN */
#if !defined A_I64_MAX && defined(INT64_MAX)
#define A_I64_MAX INT64_MAX
#endif /* A_I64_MAX */
/*! static cast to \ref a_i64 */
#define a_i64_c(x) a_cast_s(A_I64, x)
#define a_i64_(_, x) a_cast_s(A_I64 _, x)
/*! signed integer type with width of exactly 64 bits */
#define a_i64 A_I64

#if !defined A_U64
#define A_U64 uint64_t
#endif /* A_U64 */
#if !defined A_U64_C && defined(UINT64_C)
#define A_U64_C(X) UINT64_C(X)
#endif /* A_U64_C */
#if !defined A_U64_MAX && defined(UINT64_MAX)
#define A_U64_MAX UINT64_MAX
#endif /* A_U64_MAX */
/*! static cast to \ref a_u64 */
#define a_u64_c(x) a_cast_s(A_U64, x)
#define a_u64_(_, x) a_cast_s(A_U64 _, x)
/*! unsigned integer type with width of exactly 64 bits */
#define a_u64 A_U64

#if !defined A_IMAX
#define A_IMAX intmax_t
#endif /* A_IMAX */
#if !defined A_IMAX_C && defined(INTMAX_C)
#define A_IMAX_C(X) INTMAX_C(X)
#endif /* A_IMAX_C */
#if !defined A_IMAX_MIN && defined(INTMAX_MIN)
#define A_IMAX_MIN INTMAX_MIN
#endif /* A_IMAX_MIN */
#if !defined A_IMAX_MAX && defined(INTMAX_MAX)
#define A_IMAX_MAX INTMAX_MAX
#endif /* A_IMAX_MAX */
/*! static cast to \ref a_imax */
#define a_imax_c(x) a_cast_s(A_IMAX, x)
#define a_imax_(_, x) a_cast_s(A_IMAX _, x)
/*! maximum-width signed integer type */
#define a_imax A_IMAX

#if !defined A_UMAX
#define A_UMAX uintmax_t
#endif /* A_UMAX */
#if !defined A_UMAX_C && defined(UINTMAX_C)
#define A_UMAX_C(X) UINTMAX_C(X)
#endif /* A_UMAX_C */
#if !defined A_UMAX_MAX && defined(UINTMAX_MAX)
#define A_UMAX_MAX UINTMAX_MAX
#endif /* A_UMAX_MAX */
/*! static cast to \ref a_umax */
#define a_umax_c(x) a_cast_s(A_UMAX, x)
#define a_umax_(_, x) a_cast_s(A_UMAX _, x)
/*! maximum-width unsigned integer type */
#define a_umax A_UMAX

#if !defined A_IPTR
#define A_IPTR intptr_t
#endif /* A_IPTR */
#if !defined A_IPTR_MIN && defined(INTPTR_MIN)
#define A_IPTR_MIN INTPTR_MIN
#endif /* A_IPTR_MIN */
#if !defined A_IPTR_MAX && defined(INTPTR_MAX)
#define A_IPTR_MAX INTPTR_MAX
#endif /* A_IPTR_MAX */
/*! static cast to \ref a_iptr */
#define a_iptr_c(x) a_cast_s(A_IPTR, x)
#define a_iptr_(_, x) a_cast_s(A_IPTR _, x)
/*! signed integer type capable of holding a pointer to void */
#define a_iptr A_IPTR

#if !defined A_UPTR
#define A_UPTR uintptr_t
#endif /* A_UPTR */
#if !defined A_UPTR_MAX && defined(UINTPTR_MAX)
#define A_UPTR_MAX UINTPTR_MAX
#endif /* A_UPTR_MAX */
/*! static cast to \ref a_uptr */
#define a_uptr_c(x) a_cast_s(A_UPTR, x)
#define a_uptr_(_, x) a_cast_s(A_UPTR _, x)
/*! unsigned integer type capable of holding a pointer to void */
#define a_uptr A_UPTR

#if !defined A_DIFF
#define A_DIFF ptrdiff_t
#endif /* A_DIFF */
#if !defined A_DIFF_MIN && defined(PTRDIFF_MIN)
#define A_DIFF_MIN PTRDIFF_MIN
#endif /* A_DIFF_MIN */
#if !defined A_DIFF_MAX && defined(PTRDIFF_MAX)
#define A_DIFF_MAX PTRDIFF_MAX
#endif /* A_DIFF_MAX */
/*! static cast to \ref a_diff */
#define a_diff_c(x) a_cast_s(A_DIFF, x)
#define a_diff_(_, x) a_cast_s(A_DIFF _, x)
/*! signed integer type returned when subtracting two pointers */
#define a_diff A_DIFF

#if !defined A_SIZE
#define A_SIZE size_t
#endif /* A_SIZE */
#if !defined A_SIZE_MAX && defined(SIZE_MAX)
#define A_SIZE_MAX SIZE_MAX
#endif /* A_SIZE_MAX */
/*! static cast to \ref a_size */
#define a_size_c(x) a_cast_s(A_SIZE, x)
#define a_size_(_, x) a_cast_s(A_SIZE _, x)
/*! unsigned integer type returned by the sizeof operator */
#define a_size A_SIZE

#define A_F16_NNAN A_U16_C(0xFE00)
#define A_F16_PNAN A_U16_C(0x7E00)
#define A_F16_NINF A_U16_C(0xFC00)
#define A_F16_PINF A_U16_C(0x7C00)

#define A_F32 float
#define A_F32_C(X) A_CAST_2(X, F)
#define A_F32_F(F) A_CAST_2(F, f)
#define A_F32_F1(F, a) A_CAST_2(F, f)(a)
#define A_F32_F2(F, a, b) A_CAST_2(F, f)(a, b)
#define A_F32_F3(F, a, b, c) A_CAST_2(F, f)(a, b, c)
#if defined(A_HAVE_VARIADIC_MACROS)
#define A_F32_FN(F, ...) A_CAST_2(F, f)(__VA_ARGS__)
#endif /* A_HAVE_VARIADIC_MACROS */
#define A_F32_DIG FLT_DIG
#define A_F32_EPSILON FLT_EPSILON
#define A_F32_MANT_DIG FLT_MANT_DIG
#define A_F32_MAX FLT_MAX
#define A_F32_MAX_10_EXP FLT_MAX_10_EXP
#define A_F32_MAX_EXP FLT_MAX_EXP
#define A_F32_MIN FLT_MIN
#define A_F32_MIN_10_EXP FLT_MIN_10_EXP
#define A_F32_MIN_EXP FLT_MIN_EXP
#define A_F32_INF a_cast_s(A_F32, A_F64_INF)
#define A_F32_NAN (A_F32_C(0.0) * A_F32_INF)
#define A_F32_NNAN A_U32_C(0xFFC00000)
#define A_F32_PNAN A_U32_C(0x7FC00000)
#define A_F32_NINF A_U32_C(0xFF800000)
#define A_F32_PINF A_U32_C(0x7F800000)
/*! format constants for the fprintf family of functions */
#define A_F32_PRI(F, C) "%" F C
/*! format constants for the fscanf family of functions */
#define A_F32_SCN(F, C) "%" F C
/*! static cast to \ref a_f32 */
#define a_f32_c(x) a_cast_s(A_F32, x)
#define a_f32_(_, x) a_cast_s(A_F32 _, x)
/*! single precision floating point type. Matches IEEE-754 binary32 format if supported. */
#define a_f32 A_F32

#define A_F64 double
#define A_F64_C(X) X
#define A_F64_F(F) F
#define A_F64_F1(F, a) F(a)
#define A_F64_F2(F, a, b) F(a, b)
#define A_F64_F3(F, a, b, c) F(a, b, c)
#if defined(A_HAVE_VARIADIC_MACROS)
#define A_F64_FN(F, ...) F(__VA_ARGS__)
#endif /* A_HAVE_VARIADIC_MACROS */
#define A_F64_DIG DBL_DIG
#define A_F64_EPSILON DBL_EPSILON
#define A_F64_MANT_DIG DBL_MANT_DIG
#define A_F64_MAX DBL_MAX
#define A_F64_MAX_10_EXP DBL_MAX_10_EXP
#define A_F64_MAX_EXP DBL_MAX_EXP
#define A_F64_MIN DBL_MIN
#define A_F64_MIN_10_EXP DBL_MIN_10_EXP
#define A_F64_MIN_EXP DBL_MIN_EXP
#define A_F64_INF (DBL_MAX * DBL_MAX)
#define A_F64_NAN (A_F64_C(0.0) * A_F64_INF)
#define A_F64_NNAN A_U64_C(0xFFF8000000000000)
#define A_F64_PNAN A_U64_C(0x7FF8000000000000)
#define A_F64_NINF A_U64_C(0xFFF0000000000000)
#define A_F64_PINF A_U64_C(0x7FF0000000000000)
/*! format constants for the fprintf family of functions */
#define A_F64_PRI(F, C) "%" F "l" C
/*! format constants for the fscanf family of functions */
#define A_F64_SCN(F, C) "%" F "l" C
/*! static cast to \ref a_f64 */
#define a_f64_c(x) a_cast_s(A_F64, x)
#define a_f64_(_, x) a_cast_s(A_F64 _, x)
/*! double precision floating point type. Matches IEEE-754 binary64 format if supported. */
#define a_f64 A_F64

/*!
 @addtogroup a_float floating-point number
 @{
*/

/*! floating-point number bytes */
#if !defined A_FLOAT_TYPE
#if !defined A_SIZE_FLOAT
#define A_FLOAT_TYPE A_FLOAT_DOUBLE
#else /* !A_SIZE_FLOAT */
#define A_FLOAT_TYPE A_SIZE_FLOAT
#endif /* A_SIZE_FLOAT */
#endif /* A_FLOAT_TYPE */
#define A_FLOAT_SINGLE 0x04
#define A_FLOAT_DOUBLE 0x08
#define A_FLOAT_EXTEND 0x10
#if defined(A_FLOAT)
#elif A_FLOAT_TYPE + 0 == A_FLOAT_SINGLE

/*! floating-point number stored using `float` */
#define A_FLOAT float
#define A_FLOAT_DIG FLT_DIG
#define A_FLOAT_EPSILON FLT_EPSILON
#define A_FLOAT_MANT_DIG FLT_MANT_DIG
#define A_FLOAT_MAX FLT_MAX
#define A_FLOAT_MAX_10_EXP FLT_MAX_10_EXP
#define A_FLOAT_MAX_EXP FLT_MAX_EXP
#define A_FLOAT_MIN FLT_MIN
#define A_FLOAT_MIN_10_EXP FLT_MIN_10_EXP
#define A_FLOAT_MIN_EXP FLT_MIN_EXP

/*!
 expands to a floating-point constant expression having the value specified by its argument and the type \ref a_float
*/
#define A_FLOAT_C(X) A_CAST_2(X, F)
/*!
 expands to a floating-point function expression having the value specified by its argument and the type \ref a_float
*/
#define A_FLOAT_F(F) A_CAST_2(F, f)
#define A_FLOAT_F1(F, a) A_CAST_2(F, f)(a)
#define A_FLOAT_F2(F, a, b) A_CAST_2(F, f)(a, b)
#define A_FLOAT_F3(F, a, b, c) A_CAST_2(F, f)(a, b, c)
#if defined(A_HAVE_VARIADIC_MACROS)
#define A_FLOAT_FN(F, ...) A_CAST_2(F, f)(__VA_ARGS__)
#endif /* A_HAVE_VARIADIC_MACROS */

/*! format constants for the fprintf family of functions */
#define A_FLOAT_PRI(F, C) "%" F C
/*! format constants for the fscanf family of functions */
#define A_FLOAT_SCN(F, C) "%" F C

#elif A_FLOAT_TYPE + 0 == A_FLOAT_DOUBLE

/*! floating-point number stored using `double` */
#define A_FLOAT double
#define A_FLOAT_DIG DBL_DIG
#define A_FLOAT_EPSILON DBL_EPSILON
#define A_FLOAT_MANT_DIG DBL_MANT_DIG
#define A_FLOAT_MAX DBL_MAX
#define A_FLOAT_MAX_10_EXP DBL_MAX_10_EXP
#define A_FLOAT_MAX_EXP DBL_MAX_EXP
#define A_FLOAT_MIN DBL_MIN
#define A_FLOAT_MIN_10_EXP DBL_MIN_10_EXP
#define A_FLOAT_MIN_EXP DBL_MIN_EXP

/*!
 expands to a floating-point constant expression having the value specified by its argument and the type \ref a_float
*/
#define A_FLOAT_C(X) X
/*!
 expands to a floating-point function expression having the value specified by its argument and the type \ref a_float
*/
#define A_FLOAT_F(F) F
#define A_FLOAT_F1(F, a) F(a)
#define A_FLOAT_F2(F, a, b) F(a, b)
#define A_FLOAT_F3(F, a, b, c) F(a, b, c)
#if defined(A_HAVE_VARIADIC_MACROS)
#define A_FLOAT_FN(F, ...) F(__VA_ARGS__)
#endif /* A_HAVE_VARIADIC_MACROS */

/*! format constants for the fprintf family of functions */
#define A_FLOAT_PRI(F, C) "%" F C
/*! format constants for the fscanf family of functions */
#define A_FLOAT_SCN(F, C) "%" F "l" C

#elif A_FLOAT_TYPE + 0 == A_FLOAT_EXTEND

/*! floating-point number stored using `long double` */
#define A_FLOAT long double
#define A_FLOAT_DIG LDBL_DIG
#define A_FLOAT_EPSILON LDBL_EPSILON
#define A_FLOAT_MANT_DIG LDBL_MANT_DIG
#define A_FLOAT_MAX LDBL_MAX
#define A_FLOAT_MAX_10_EXP LDBL_MAX_10_EXP
#define A_FLOAT_MAX_EXP LDBL_MAX_EXP
#define A_FLOAT_MIN LDBL_MIN
#define A_FLOAT_MIN_10_EXP LDBL_MIN_10_EXP
#define A_FLOAT_MIN_EXP LDBL_MIN_EXP

/*!
 expands to a floating-point constant expression having the value specified by its argument and the type \ref a_float
*/
#define A_FLOAT_C(X) A_CAST_2(X, L)
/*!
 expands to a floating-point function expression having the value specified by its argument and the type \ref a_float
*/
#define A_FLOAT_F(F) A_CAST_2(F, l)
#define A_FLOAT_F1(F, a) A_CAST_2(F, l)(a)
#define A_FLOAT_F2(F, a, b) A_CAST_2(F, l)(a, b)
#define A_FLOAT_F3(F, a, b, c) A_CAST_2(F, l)(a, b, c)
#if defined(A_HAVE_VARIADIC_MACROS)
#define A_FLOAT_FN(F, ...) A_CAST_2(F, l)(__VA_ARGS__)
#endif /* A_HAVE_VARIADIC_MACROS */

/*! format constants for the fprintf family of functions */
#define A_FLOAT_PRI(F, C) "%" F "L" C
/*! format constants for the fscanf family of functions */
#define A_FLOAT_SCN(F, C) "%" F "L" C

#else /* !A_FLOAT_TYPE */
#error unsupported precision
#endif /* A_FLOAT_TYPE */

#define A_FLOAT_INF a_cast_s(A_FLOAT, A_F64_INF)
#define A_FLOAT_NAN (A_FLOAT_C(0.0) * A_FLOAT_INF)

/*! static cast to \ref a_float */
#define a_float_c(x) a_cast_s(A_FLOAT, x)
#define a_float_(_, x) a_cast_s(A_FLOAT _, x)
/*! compiler built-in floating-point number type */
#define a_float A_FLOAT

/*! @} a_float */

typedef union a_cast
{
    a_c8 c;
    a_int i;
    a_uint u;
    a_shrt ih;
    a_ushrt uh;
    a_long il;
    a_ulong ul;
#if defined(A_HAVE_LONG_LONG_TYPE)
    a_llong ill;
    a_ullong ull;
#endif /* A_HAVE_LONG_LONG_TYPE */
    a_i8 i8;
    a_u8 u8;
    a_i16 i16;
    a_u16 u16;
    a_i32 i32;
    a_u32 u32;
    a_i64 i64;
    a_u64 u64;
    a_f32 f32;
    a_f64 f64;
    a_imax imax;
    a_umax umax;
    a_iptr iptr;
    a_uptr uptr;
    a_diff diff;
    a_size size;
    void const *PTR;
    void *ptr;
    char const *STR;
    char *str;
#if defined(A_FLOAT_TYPE) && (A_FLOAT_TYPE + 0 < A_FLOAT_EXTEND)
    a_float f;
#endif /* A_FLOAT_TYPE */
} a_cast;

/*!
 @brief square of x, \f$ x^2 \f$
*/
#define A_SQ(x) ((x) * (x))

/*!
 @brief absolute value of x, \f$ |x| \f$
*/
#define A_ABS(x) ((x) < 0 ? -(x) : (x))

/*!
 @brief minimum value between x and y
*/
#define A_MIN(x, y) (((x) < (y)) ? (x) : (y))

/*!
 @brief maximum value between x and y
*/
#define A_MAX(x, y) (((x) > (y)) ? (x) : (y))

/*!
 @brief signum function, \f$ \texttt{sgn}{x}=\begin{cases}+1&x>0\\0&0\\-1&x<0\end{cases} \f$
*/
#define A_SGN(x) ((0 < (x)) - ((x) < 0))

/*!
 @brief saturation value of x, \f$ \texttt{sat}(x,min,max)=\begin{cases}min&min>x\\max&x>max\\x&else\end{cases} \f$
*/
#define A_SAT(x, min, max) ((min) < (x) ? (x) < (max) ? (x) : (max) : (min))

/*!
 @brief number of elements in a visible array
 @param a must be a visible array
*/
#define A_LEN(a) (sizeof(a) / sizeof(*(a)))

/*!
 @brief offset of a structure member
 @param type structure type
 @param member member variable
*/
#if defined(offsetof)
#define a_offsetof(type, member) offsetof(type, member)
#else /* !offsetof */
#define a_offsetof(type, member) a_cast_r(a_size, &a_cast_r(type *, 0)->member)
#endif /* offsetof */

/*!
 @brief container of a structure member
 @param ptr pointer to a member variable
 @param type structure type
 @param member member variable
*/
#define a_container_of(ptr, type, member) a_cast_r(type *, a_cast_r(a_uptr, ptr) - a_offsetof(type, member))

/*!
 @brief round down size `n` to be a multiple of `a`
*/
#define a_size_down(a, n) (a_cast_s(a_size, n) & ~a_cast_s(a_size, (a) - 1))

/*!
 @brief round up size `n` to be a multiple of `a`
*/
#define a_size_up(a, n) ((a_cast_s(a_size, n) + (a) - 1) & ~a_cast_s(a_size, (a) - 1))

/*!
 @brief round pointer `p` down to the closest `a`, aligned address <= `p`
*/
#define a_align_down(a, p) a_cast_r(void *, a_cast_r(a_uptr, p) & ~a_cast_s(a_uptr, (a) - 1))

/*!
 @brief round pointer `p` up to the closest `a`, aligned address >= `p`
*/
#define a_align_up(a, p) a_cast_r(void *, (a_cast_r(a_uptr, p) + (a) - 1) & ~a_cast_s(a_uptr, (a) - 1))

/*!
 @brief iterate from 0 to n and not include n
 @param I index type of the iteration
 @param i index variable of the iteration
 @param n final value of the iteration
*/
#define a_forenum(I, i, n) for (I i = 0; i < (n); ++i)

/*!
 @brief iterate from n to 0 and not include n
 @param I index type of the iteration
 @param i index variable of the iteration
 @param n final value of the iteration
*/
#define a_forenum_reverse(I, i, n) for (I i = (n); i-- > 0;)

/*!
 @brief iterate over an array
 @param T the prefix of the element type
 @param P the suffix of the element type
 @param it pointer to the current element
 @param ptr starting address of this array
 @param num number of elements in this array
*/
#define a_foreach(T, P, it, ptr, num) \
    for (T P it = a_cast_s(T P, ptr), P it##_ = it + (num); it < it##_; ++it)
/*! @copydoc a_foreach */
#define a_forsafe(T, P, it, ptr, num) \
    for (T P it = a_cast_s(T P, ptr), P it##_ = (num) ? it + (num) : it; it < it##_; ++it)

/*!
 @brief iterate over an array in reverse
 @param T the prefix of the element type
 @param P the suffix of the element type
 @param it pointer to the current element
 @param ptr starting address of this array
 @param num number of elements in this array
*/
#define a_foreach_reverse(T, P, it, ptr, num) \
    for (T P it##_ = a_cast_s(T P, ptr) - 1, P it = it##_ + (num); it > it##_; --it)
/*! @copydoc a_foreach_reverse */
#define a_forsafe_reverse(T, P, it, ptr, num)                             \
    for (T P it##_ = (num) ? a_cast_s(T P, ptr) - 1 : a_cast_s(T P, ptr), \
             P it = (num) ? it##_ + (num) : it##_;                        \
         it > it##_; --it)

/*!
 @brief iterate over an array
 @param T the prefix of the element type
 @param P the suffix of the element type
 @param it pointer to the current element
 @param ptr starting address of this array
 @param end the end address of this array
*/
#define a_iterate(T, P, it, ptr, end) \
    for (T P it = a_cast_s(T P, ptr), P it##_ = a_cast_s(T P, end); it < it##_; ++it)

/*!
 @brief iterate over an array in reverse
 @param T the prefix of the element type
 @param P the suffix of the element type
 @param it pointer to the current element
 @param ptr starting address of this array
 @param end the end address of this array
*/
#define a_iterate_reverse(T, P, it, ptr, end) \
    for (T P it = a_cast_s(T P, end) - 1, P it##_ = a_cast_s(T P, ptr) - 1; it > it##_; --it)

/*!
 @brief enumeration for return values
*/
enum
{
    A_SUCCESS /*!< return success */,
    A_FAILURE /*!< return failure */,
    A_INVALID /*!< return invalid */
};

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */
#if defined(LIBA_A_C)
#undef A_INTERN
#define A_INTERN A_INLINE
#endif /* LIBA_A_C */

/*!
 @brief reverse the bits in an 8-bit unsigned integer
 @param x an 8-bit unsigned integer to be reversed
 @return reversed 8-bit unsigned integer
*/
#if !defined A_HAVE_INLINE || defined(LIBA_A_C)
A_EXTERN a_u8 a_u8_rev(a_u8 x);
#endif /* A_HAVE_INLINE */
#if defined(A_HAVE_INLINE) || defined(LIBA_A_C)
A_INTERN a_u8 a_u8_rev(a_u8 x)
{
    x = a_cast_s(a_u8, (x >> 4) | (x << 4));
    x = a_cast_s(a_u8, ((x & 0xCC) >> 2) | ((x & 0x33) << 2));
    x = a_cast_s(a_u8, ((x & 0xAA) >> 1) | ((x & 0x55) << 1));
    return x;
}
#endif /* A_HAVE_INLINE */

/*!
 @brief reverse the bits in a 16-bit unsigned integer
 @param x a 16-bit unsigned integer to be reversed
 @return reversed 16-bit unsigned integer
*/
#if !defined A_HAVE_INLINE || defined(LIBA_A_C)
A_EXTERN a_u16 a_u16_rev(a_u16 x);
#endif /* A_HAVE_INLINE */
#if defined(A_HAVE_INLINE) || defined(LIBA_A_C)
A_INTERN a_u16 a_u16_rev(a_u16 x)
{
    x = a_cast_s(a_u16, (x >> 8) | (x << 8));
    x = a_cast_s(a_u16, ((x & 0xF0F0) >> 4) | ((x & 0x0F0F) << 4));
    x = a_cast_s(a_u16, ((x & 0xCCCC) >> 2) | ((x & 0x3333) << 2));
    x = a_cast_s(a_u16, ((x & 0xAAAA) >> 1) | ((x & 0x5555) << 1));
    return x;
}
#endif /* A_HAVE_INLINE */
#if !defined A_HAVE_INLINE || defined(LIBA_A_C)
A_EXTERN A_NONULL((1)) a_u16 a_u16_getl(void const *b);
#endif /* A_HAVE_INLINE */
#if defined(A_HAVE_INLINE) || defined(LIBA_A_C)
A_INTERN A_NONULL((1)) a_u16 a_u16_getl(void const *b)
{
    a_byte const *p = a_cast_s(a_byte const *, b);
    return a_cast_s(a_u16, (p[0] << 0) | (p[1] << 8));
}
#endif /* A_HAVE_INLINE */
#if !defined A_HAVE_INLINE || defined(LIBA_A_C)
A_EXTERN A_NONULL((1)) a_u16 a_u16_getb(void const *b);
#endif /* A_HAVE_INLINE */
#if defined(A_HAVE_INLINE) || defined(LIBA_A_C)
A_INTERN A_NONULL((1)) a_u16 a_u16_getb(void const *b)
{
    a_byte const *p = a_cast_s(a_byte const *, b);
    return a_cast_s(a_u16, (p[1] << 0) | (p[0] << 8));
}
#endif /* A_HAVE_INLINE */
#if !defined A_HAVE_INLINE || defined(LIBA_A_C)
A_EXTERN A_NONULL((1)) void a_u16_setl(void *b, a_u16 x);
#endif /* A_HAVE_INLINE */
#if defined(A_HAVE_INLINE) || defined(LIBA_A_C)
A_INTERN A_NONULL((1)) void a_u16_setl(void *b, a_u16 x)
{
    a_byte *p = a_cast_s(a_byte *, b);
    p[0] = a_cast_s(a_byte, x >> 0);
    p[1] = a_cast_s(a_byte, x >> 8);
}
#endif /* A_HAVE_INLINE */
#if !defined A_HAVE_INLINE || defined(LIBA_A_C)
A_EXTERN A_NONULL((1)) void a_u16_setb(void *b, a_u16 x);
#endif /* A_HAVE_INLINE */
#if defined(A_HAVE_INLINE) || defined(LIBA_A_C)
A_INTERN A_NONULL((1)) void a_u16_setb(void *b, a_u16 x)
{
    a_byte *p = a_cast_s(a_byte *, b);
    p[0] = a_cast_s(a_byte, x >> 8);
    p[1] = a_cast_s(a_byte, x >> 0);
}
#endif /* A_HAVE_INLINE */

/*!
 @brief reverse the bits in a 32-bit unsigned integer
 @param x a 32-bit unsigned integer to be reversed
 @return reversed 32-bit unsigned integer
*/
#if !defined A_HAVE_INLINE || defined(LIBA_A_C)
A_EXTERN a_u32 a_u32_rev(a_u32 x);
#endif /* A_HAVE_INLINE */
#if defined(A_HAVE_INLINE) || defined(LIBA_A_C)
A_INTERN a_u32 a_u32_rev(a_u32 x)
{
    x = (x >> 16) | (x << 16);
    x = ((x & 0xFF00FF00) >> 8) | ((x & 0x00FF00FF) << 8);
    x = ((x & 0xF0F0F0F0) >> 4) | ((x & 0x0F0F0F0F) << 4);
    x = ((x & 0xCCCCCCCC) >> 2) | ((x & 0x33333333) << 2);
    x = ((x & 0xAAAAAAAA) >> 1) | ((x & 0x55555555) << 1);
    return x;
}
#endif /* A_HAVE_INLINE */
#if !defined A_HAVE_INLINE || defined(LIBA_A_C)
A_EXTERN A_NONULL((1)) a_u32 a_u32_getl(void const *b);
#endif /* A_HAVE_INLINE */
#if defined(A_HAVE_INLINE) || defined(LIBA_A_C)
A_INTERN A_NONULL((1)) a_u32 a_u32_getl(void const *b)
{
    a_byte const *p = a_cast_s(a_byte const *, b);
    return (a_cast_s(a_u32, p[0]) << 0x00) |
           (a_cast_s(a_u32, p[1]) << 0x08) |
           (a_cast_s(a_u32, p[2]) << 0x10) |
           (a_cast_s(a_u32, p[3]) << 0x18);
}
#endif /* A_HAVE_INLINE */
#if !defined A_HAVE_INLINE || defined(LIBA_A_C)
A_EXTERN A_NONULL((1)) a_u32 a_u32_getb(void const *b);
#endif /* A_HAVE_INLINE */
#if defined(A_HAVE_INLINE) || defined(LIBA_A_C)
A_INTERN A_NONULL((1)) a_u32 a_u32_getb(void const *b)
{
    a_byte const *p = a_cast_s(a_byte const *, b);
    return (a_cast_s(a_u32, p[0]) << 0x18) |
           (a_cast_s(a_u32, p[1]) << 0x10) |
           (a_cast_s(a_u32, p[2]) << 0x08) |
           (a_cast_s(a_u32, p[3]) << 0x00);
}
#endif /* A_HAVE_INLINE */
#if !defined A_HAVE_INLINE || defined(LIBA_A_C)
A_EXTERN A_NONULL((1)) void a_u32_setl(void *b, a_u32 x);
#endif /* A_HAVE_INLINE */
#if defined(A_HAVE_INLINE) || defined(LIBA_A_C)
A_INTERN A_NONULL((1)) void a_u32_setl(void *b, a_u32 x)
{
    a_byte *p = a_cast_s(a_byte *, b);
    p[0] = a_cast_s(a_byte, x >> 0x00);
    p[1] = a_cast_s(a_byte, x >> 0x08);
    p[2] = a_cast_s(a_byte, x >> 0x10);
    p[3] = a_cast_s(a_byte, x >> 0x18);
}
#endif /* A_HAVE_INLINE */
#if !defined A_HAVE_INLINE || defined(LIBA_A_C)
A_EXTERN A_NONULL((1)) void a_u32_setb(void *b, a_u32 x);
#endif /* A_HAVE_INLINE */
#if defined(A_HAVE_INLINE) || defined(LIBA_A_C)
A_INTERN A_NONULL((1)) void a_u32_setb(void *b, a_u32 x)
{
    a_byte *p = a_cast_s(a_byte *, b);
    p[0] = a_cast_s(a_byte, x >> 0x18);
    p[1] = a_cast_s(a_byte, x >> 0x10);
    p[2] = a_cast_s(a_byte, x >> 0x08);
    p[3] = a_cast_s(a_byte, x >> 0x00);
}
#endif /* A_HAVE_INLINE */

/*!
 @brief reverse the bits in a 64-bit unsigned integer
 @param x a 64-bit unsigned integer to be reversed
 @return reversed 64-bit unsigned integer
*/
#if !defined A_HAVE_INLINE || defined(LIBA_A_C)
A_EXTERN a_u64 a_u64_rev(a_u64 x);
#endif /* A_HAVE_INLINE */
#if defined(A_HAVE_INLINE) || defined(LIBA_A_C)
A_INTERN a_u64 a_u64_rev(a_u64 x)
{
    x = (x >> 32) | (x << 32);
    x = ((x & 0xFFFF0000FFFF0000) >> 0x10) | ((x & 0x0000FFFF0000FFFF) << 0x10);
    x = ((x & 0xFF00FF00FF00FF00) >> 0x08) | ((x & 0x00FF00FF00FF00FF) << 0x08);
    x = ((x & 0xF0F0F0F0F0F0F0F0) >> 0x04) | ((x & 0x0F0F0F0F0F0F0F0F) << 0x04);
    x = ((x & 0xCCCCCCCCCCCCCCCC) >> 0x02) | ((x & 0x3333333333333333) << 0x02);
    x = ((x & 0xAAAAAAAAAAAAAAAA) >> 0x01) | ((x & 0x5555555555555555) << 0x01);
    return x;
}
#endif /* A_HAVE_INLINE */
#if !defined A_HAVE_INLINE || defined(LIBA_A_C)
A_EXTERN A_NONULL((1)) a_u64 a_u64_getl(void const *b);
#endif /* A_HAVE_INLINE */
#if defined(A_HAVE_INLINE) || defined(LIBA_A_C)
A_INTERN A_NONULL((1)) a_u64 a_u64_getl(void const *b)
{
    a_byte const *p = a_cast_s(a_byte const *, b);
    return (a_cast_s(a_u64, p[0]) << 0x00) |
           (a_cast_s(a_u64, p[1]) << 0x08) |
           (a_cast_s(a_u64, p[2]) << 0x10) |
           (a_cast_s(a_u64, p[3]) << 0x18) |
           (a_cast_s(a_u64, p[4]) << 0x20) |
           (a_cast_s(a_u64, p[5]) << 0x28) |
           (a_cast_s(a_u64, p[6]) << 0x30) |
           (a_cast_s(a_u64, p[7]) << 0x38);
}
#endif /* A_HAVE_INLINE */
#if !defined A_HAVE_INLINE || defined(LIBA_A_C)
A_EXTERN A_NONULL((1)) a_u64 a_u64_getb(void const *b);
#endif /* A_HAVE_INLINE */
#if defined(A_HAVE_INLINE) || defined(LIBA_A_C)
A_INTERN A_NONULL((1)) a_u64 a_u64_getb(void const *b)
{
    a_byte const *p = a_cast_s(a_byte const *, b);
    return (a_cast_s(a_u64, p[0]) << 0x38) |
           (a_cast_s(a_u64, p[1]) << 0x30) |
           (a_cast_s(a_u64, p[2]) << 0x28) |
           (a_cast_s(a_u64, p[3]) << 0x20) |
           (a_cast_s(a_u64, p[4]) << 0x18) |
           (a_cast_s(a_u64, p[5]) << 0x10) |
           (a_cast_s(a_u64, p[6]) << 0x08) |
           (a_cast_s(a_u64, p[7]) << 0x00);
}
#endif /* A_HAVE_INLINE */
#if !defined A_HAVE_INLINE || defined(LIBA_A_C)
A_EXTERN A_NONULL((1)) void a_u64_setl(void *b, a_u64 x);
#endif /* A_HAVE_INLINE */
#if defined(A_HAVE_INLINE) || defined(LIBA_A_C)
A_INTERN A_NONULL((1)) void a_u64_setl(void *b, a_u64 x)
{
    a_byte *p = a_cast_s(a_byte *, b);
    p[0] = a_cast_s(a_byte, x >> 0x00);
    p[1] = a_cast_s(a_byte, x >> 0x08);
    p[2] = a_cast_s(a_byte, x >> 0x10);
    p[3] = a_cast_s(a_byte, x >> 0x18);
    p[4] = a_cast_s(a_byte, x >> 0x20);
    p[5] = a_cast_s(a_byte, x >> 0x28);
    p[6] = a_cast_s(a_byte, x >> 0x30);
    p[7] = a_cast_s(a_byte, x >> 0x38);
}
#endif /* A_HAVE_INLINE */
#if !defined A_HAVE_INLINE || defined(LIBA_A_C)
A_EXTERN A_NONULL((1)) void a_u64_setb(void *b, a_u64 x);
#endif /* A_HAVE_INLINE */
#if defined(A_HAVE_INLINE) || defined(LIBA_A_C)
A_INTERN A_NONULL((1)) void a_u64_setb(void *b, a_u64 x)
{
    a_byte *p = a_cast_s(a_byte *, b);
    p[0] = a_cast_s(a_byte, x >> 0x38);
    p[1] = a_cast_s(a_byte, x >> 0x30);
    p[2] = a_cast_s(a_byte, x >> 0x28);
    p[3] = a_cast_s(a_byte, x >> 0x20);
    p[4] = a_cast_s(a_byte, x >> 0x18);
    p[5] = a_cast_s(a_byte, x >> 0x10);
    p[6] = a_cast_s(a_byte, x >> 0x08);
    p[7] = a_cast_s(a_byte, x >> 0x00);
}
#endif /* A_HAVE_INLINE */

/*!
 @brief copy one buffer to another
 @param[in] dst pointer to the object to copy to
 @param[in] src pointer to the object to copy from
 @param[in] siz number of bytes to copy
 @return a copy of dest
*/
A_EXTERN A_NONULL((1, 2)) void *a_copy(void *__restrict dst, void const *__restrict src, a_size siz);

/*!
 @brief move one buffer to another
 @param[in] dst pointer to the object to copy to
 @param[in] src pointer to the object to copy from
 @param[in] siz number of bytes to copy
 @return a copy of dest
*/
A_EXTERN A_NONULL((1, 2)) void *a_move(void *dst, void const *src, a_size siz);

/*!
 @brief fill a buffer with a character
 @param[in] ptr pointer to the object to fill
 @param[in] siz number of bytes to fill
 @param[in] val fill byte
 @return a copy of dest
*/
A_EXTERN A_NONULL((1)) void *a_fill(void *ptr, a_size siz, int val);

/*!
 @brief fill a buffer with zero
 @param[in] ptr pointer to the object to fill
 @param[in] siz number of bytes to fill
 @return a copy of dest
*/
A_EXTERN A_NONULL((1)) void *a_zero(void *ptr, a_size siz);

/*!
 @brief swap two different memory blocks of the same size
 @param[in] siz the size of memory block being swapped
 @param[in,out] lhs points to memory block on the left
 @param[in,out] rhs points to memory block on the right
*/
A_EXTERN A_NONULL((1, 2)) void a_swap(void *lhs, void *rhs, a_size siz);

/*!
 @brief a hash function whose prime number is 131
 @param[in] str string to be processed
 @param[in] val initial value
 @return hash value
*/
A_EXTERN a_u32 a_hash_bkdr(void const *str, a_u32 val);

/*!
 @brief a hash function whose prime number is 131
 @param[in] ptr points to string to be processed
 @param[in] siz length of string to be processed
 @param[in] val initial value
 @return hash value
*/
A_EXTERN a_u32 a_hash_bkdr_(void const *ptr, a_size siz, a_u32 val);

/*!
 @brief a hash function whose prime number is 65599
 @param[in] str string to be processed
 @param[in] val initial value
 @return hash value
*/
A_EXTERN a_u32 a_hash_sdbm(void const *str, a_u32 val);

/*!
 @brief a hash function whose prime number is 65599
 @param[in] ptr points to string to be processed
 @param[in] siz length of string to be processed
 @param[in] val initial value
 @return hash value
*/
A_EXTERN a_u32 a_hash_sdbm_(void const *ptr, a_size siz, a_u32 val);

/*!
 @brief roll back the elements of a float array and save the cache array
 @param[in] block_p points to a float array
 @param[in] block_n number of a float array
 @param[in] cache_p points to a cache array
 @param[in] cache_n number of a cache array
*/
A_EXTERN void a_float_push(a_float *block_p, a_size block_n,
                           a_float const *cache_p, a_size cache_n);

/*!
 @brief roll back the elements of a float array circularly, array>>shift
 @param[in] block_p points to a float array
 @param[in] block_n number of a float array
 @param[in] shift_p points to a shift array
 @param[in] shift_n number of a shift array
*/
A_EXTERN void a_float_roll(a_float *block_p, a_size block_n,
                           a_float *shift_p, a_size shift_n);

/*!
 @brief allocation function pointer
 @param[in] addr address of memory block
 @param[in] size new size of memory block
 @return new address of memory block or null
*/
A_EXTERN void *(*a_alloc)(void *addr, a_size size);

/*!
 @brief default allocation function
 @param[in] addr address of memory block
 @param[in] size new size of memory block
 @return new address of memory block or null
*/
A_EXTERN void *a_alloc_(void *addr, a_size size);

#if defined(LIBA_A_C)
#undef A_INTERN
#define A_INTERN static A_INLINE
#endif /* LIBA_A_C */
#if defined(__cplusplus)
} /* extern "C" */
#endif /* __cplusplus */

/*! @brief declare allocation function */
#define A_ALLOC(alloc, addr, size) void *alloc(void *addr, a_size size)
#define a_new(T, ptr, num) a_cast_s(T *, a_alloc(ptr, sizeof(T) * (num)))
#define a_die(ptr) a_alloc(ptr, 0)

/*! @} liba */

#endif /* a/a.h */
