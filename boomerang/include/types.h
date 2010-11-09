/*
 * types.h: some often used basic type definitions
 * $Revision: 1.10 $
 */
#ifndef __TYPES_H__
#define __TYPES_H__

#include <inttypes.h>

// Machine types
typedef uint8_t			Byte;		/* 8 bits */
typedef uint16_t		SWord;		/* 16 bits */
typedef uint32_t		DWord;		/* 32 bits */
typedef uint32_t		dword;		/* 32 bits */
typedef uint32_t		Word;		/* 32 bits */
typedef uint32_t		ADDRESS;	/* 32-bit unsigned */


#define STD_SIZE	32					// Standard size
// Note: there is a known name collision with NO_ADDRESS in WinSock.h
#ifdef NO_ADDRESS
#undef NO_ADDRESS
#endif
#define NO_ADDRESS ((ADDRESS)-1)		// For invalid ADDRESSes

#ifndef _MSC_VER
typedef long unsigned long QWord;		// 64 bits
#else
typedef uint64_t QWord;
#endif

#if defined(_MSC_VER)
#pragma warning(disable:4390)
#endif

#if defined(_MSC_VER) && _MSC_VER <= 1200
// For MSVC 5 or 6: warning about debug into truncated to 255 chars
#pragma warning(disable:4786)
#endif

#endif	// #ifndef __TYPES_H__
