/*
 * types.h: some often used basic type definitions
 * $Revision$
 */
#ifndef __TYPES_H__
#define __TYPES_H__

#include "config.h"
#ifdef HAVE_STDDEF_H
#include <stddef.h>
#endif
#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif

// int8_t
#ifndef INT8_MAX
typedef signed char			int8_t;
typedef unsigned char		uint8_t;
#endif

// int16_t
#ifndef INT16_MAX
typedef short				int16_t;
typedef unsigned short		uint16_t;
#endif

// int32_t
#ifndef INT32_MAX
#if SIZEOF_INT == 4
typedef int					int32_t;
typedef unsigned int		uint32_t;
#else
// die
#endif
#endif

// int64_t
#ifndef INT64_MAX
#ifndef _MSC_VER
#if SIZEOF_LONG_LONG == 8
typedef long long			int64_t;
typedef long unsigned long	uint64_t;
#else
// die
#endif
#else
typedef __int64				int64_t;
typedef unsigned __int64	uint64_t;
#endif
#endif

// Machine types
typedef uint8_t				Byte;		/* 8 bits */
typedef uint16_t			SWord;		/* 16 bits */
typedef uint32_t			DWord;		/* 32 bits */
typedef uint32_t			dword;		/* 32 bits */
typedef uint32_t			Word;		/* 32 bits */
typedef uint64_t			QWord;		/* 64 bits */
typedef uintptr_t			ADDRESS;	/* same size as pointer */


#define STD_SIZE	32					// Standard size
// Note: there is a known name collision with NO_ADDRESS in WinSock.h
#ifdef NO_ADDRESS
#undef NO_ADDRESS
#endif
#define NO_ADDRESS ((ADDRESS)-1)		// For invalid ADDRESSes

#if defined(_MSC_VER)
#pragma warning(disable:4390)
#endif

#if defined(_MSC_VER) && _MSC_VER <= 1200
// For MSVC 5 or 6: warning about debug into truncated to 255 chars
#pragma warning(disable:4786)
#endif

#endif	// #ifndef __TYPES_H__
