/*
 * Basic type
 */
#ifndef AERO_BASIC_TYPE_H_INCLUDED
#define AERO_BASIC_TYPE_H_INCLUDED

// intptr_t
#if defined(_MSC_VER) && _MSC_VER < 1600 /* VS 2010 has cstdint */
#include <crtdefs.h>
#else
#include <cstdint>
#endif

#include <boost/cstdint.hpp>
#include <boost/type_traits.hpp>

/* Provide definitions for some commonly used macros.
 *  Some of them are only provided if they haven't already
 *  been defined. It is assumed that if they are already
 *  defined then the current definition is correct.
 */
#ifndef NULL
#  ifdef __cplusplus
#    define NULL        (0)
#  else /* !__cplusplus */
#    define NULL        ((void *)0)
#  endif /* !__cplusplus */
#endif

#ifndef	FALSE
#define	FALSE	(0)
#endif

#ifndef	TRUE
#define	TRUE	(!(0))
#endif

namespace aero
{
	typedef intptr_t nint_t;
	typedef uintptr_t nuint_t;
	typedef intptr_t ssize_t;
	typedef uintptr_t size_t;

	typedef nint_t nbool_t;

	typedef char   char_t;
	typedef short  short_t;
	typedef long   long_t;
	typedef int    int_t;

	typedef unsigned char   uchar_t;
	typedef unsigned short  ushort_t;
	typedef unsigned long   ulong_t;
	typedef unsigned int    uint_t;

	typedef float   float_t;
	typedef double  double_t;
	
	typedef boost::int8_t s8;
	typedef boost::uint8_t u8;
	typedef boost::int16_t s16;
	typedef boost::uint16_t u16;
	typedef boost::int32_t s32;
	typedef boost::uint32_t u32;
	typedef boost::int64_t s64;
	typedef boost::uint64_t u64;
}

#if defined(__GNUC__)  && __GNUC__ >= 4
#  define AERO_STRUCT_OFFSET(struct_type, member) \
      ((intptr_t) offsetof (struct_type, member))
#else
#  define AERO_STRUCT_OFFSET(struct_type, member)	\
      ((intptr_t) ((boost::uint8_t*) &((struct_type*) 0)->member))
#endif

#define AERO_STRUCT_MEMBER_P(struct_p, struct_offset)   \
    ((boost::uint8_t*) (struct_p) + (aero::long_t) (struct_offset))
#define AERO_STRUCT_MEMBER(member_type, struct_p, struct_offset)   \
    (*(member_type*) AERO_STRUCT_MEMBER_P ((struct_p), (struct_offset)))
	
#define AERO_R64(x) (*(volatile boost::uint64_t *)(x))
#define AERO_R32(x) (*(volatile boost::uint32_t *)(x))
#define AERO_R16(x) (*(volatile boost::uint16_t *)(x))
#define AERO_R8(x) (*(volatile boost::uint8_t *)(x))

#endif // AERO_BASIC_TYPE_H_INCLUDED
