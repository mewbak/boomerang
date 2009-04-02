/* include/config.h.in.  Generated from configure.ac by autoheader.  */

/* Define if building universal (internal helper macro) */
#cmakedefine AC_APPLE_UNIVERSAL_BUILD

/* Define to 1 if your C++ compiler doesn't accept -c and -o together. */
#cmakedefine CXX_NO_MINUS_C_MINUS_O

/* Define to 1 if you have the <byteswap.h> header file. */
#cmakedefine HAVE_BYTESWAP_H

/* Define to 1 if you have the <dlfcn.h> header file. */
#cmakedefine HAVE_DLFCN_H

/* Define to 1 if you have the <fcntl.h> header file. */
#cmakedefine HAVE_FCNTL_H

/* Define to 1 if you have the <gc.h> header file. */
#cmakedefine HAVE_GC_H

/* Define to 1 if you have the <inttypes.h> header file. */
#cmakedefine HAVE_INTTYPES_H

/* Define to 1 if you have the `cppunit' library (-lcppunit). */
#cmakedefine HAVE_LIBCPPUNIT

/* Define to 1 if you have the `dl' library (-ldl). */
#cmakedefine HAVE_LIBDL

/* Define to 1 if you have the `expat' library (-lexpat). */
#cmakedefine HAVE_LIBEXPAT

/* Define to 1 if you have the `stdc++' library (-lstdc++). */
#cmakedefine HAVE_LIBSTDC__

/* Define to 1 if you have the <unistd.h> header file. */
#cmakedefine HAVE_UNISTD_H

/* Define to 1 if you have the <malloc.h> header file. */
#cmakedefine HAVE_MALLOC_H

/* Define to 1 if you have the <memory.h> header file. */
#cmakedefine HAVE_MEMORY_H

/* Define to 1 if you have the <stdint.h> header file. */
#cmakedefine HAVE_STDINT_H

/* Define to 1 if you have the <stdlib.h> header file. */
#cmakedefine HAVE_STDLIB_H

/* Define to 1 if you have the <strings.h> header file. */
#cmakedefine HAVE_STRINGS_H

/* Define to 1 if you have the <string.h> header file. */
#cmakedefine HAVE_STRING_H

/* Define to 1 if you have the <sys/stat.h> header file. */
#cmakedefine HAVE_SYS_STAT_H

/* Define to 1 if you have the <sys/time.h> header file. */
#cmakedefine HAVE_SYS_TIME_H

/* Define to 1 if you have the <sys/types.h> header file. */
#cmakedefine HAVE_SYS_TYPES_H

/* Define to 1 if you have the <unistd.h> header file. */
#cmakedefine HAVE_UNISTD_H

/* Define to 1 if you have the </opt/local/include/gc/gc.h> header file. */
#cmakedefine HAVE__OPT_LOCAL_INCLUDE_GC_GC_H

/* Define to 1 if you have the </opt/local/include/gc.h> header file. */
#cmakedefine HAVE__OPT_LOCAL_INCLUDE_GC_H

/* Define to 1 if you have the </sw/include/gc.h> header file. */
#cmakedefine HAVE__SW_INCLUDE_GC_H

/* Define to 1 if you have the </usr/include/gc/gc.h> header file. */
#cmakedefine HAVE__USR_INCLUDE_GC_GC_H

/* Define to 1 if you have the </usr/include/gc.h> header file. */
#cmakedefine HAVE__USR_INCLUDE_GC_H

/* Define to 1 if you have the </usr/local/include/gc.h> header file. */
#cmakedefine HAVE__USR_LOCAL_INCLUDE_GC_H

/* Host uses GNU ld */
#undef HOST_GNU_LD

/* Non zero if host machine is Mac OS X */
#undef HOST_OSX

/* Name of package */
#undef PACKAGE

/* Define to the address where bug reports for this package should be sent. */
#undef PACKAGE_BUGREPORT

/* Define to the full name of this package. */
#undef PACKAGE_NAME

/* Define to the full name and version of this package. */
#undef PACKAGE_STRING

/* Define to the one symbol short name of this package. */
#undef PACKAGE_TARNAME

/* Define to the version of this package. */
#undef PACKAGE_VERSION

/* The size of `char', as computed by sizeof. */
#define SIZEOF_CHAR @SIZEOF_CHAR@

/* The size of `double', as computed by sizeof. */
#define SIZEOF_DOUBLE @SIZEOF_DOUBLE@

/* The size of `float', as computed by sizeof. */
#define SIZEOF_FLOAT @SIZEOF_FLOAT@

/* The size of `int', as computed by sizeof. */
#define SIZEOF_INT @SIZEOF_INT@

/* The size of `int *', as computed by sizeof. */
#define SIZEOF_INT_P @SIZEOF_INT_P@

/* The size of `long', as computed by sizeof. */
#define SIZEOF_LONG @SIZEOF_LONG@

/* The size of `long double', as computed by sizeof. */
#define SIZEOF_LONG_DOUBLE @SIZEOF_LONG_DOUBLE@

/* The size of `long long', as computed by sizeof. */
#define SIZEOF_LONG_LONG @SIZEOF_LONG_LONG@

/* The size of `short', as computed by sizeof. */
#define SIZEOF_SHORT @SIZEOF_SHORT@

/* Define to 1 if you have the ANSI C header files. */
#undef STDC_HEADERS

/* Version number of package */
#undef VERSION

/* Define WORDS_BIGENDIAN to 1 if your processor stores words with the most
   significant byte first (like Motorola and SPARC, unlike Intel). */
#if defined AC_APPLE_UNIVERSAL_BUILD
# if defined __BIG_ENDIAN__
#  define WORDS_BIGENDIAN 1
# endif
#else
# ifndef WORDS_BIGENDIAN
#  undef WORDS_BIGENDIAN
# endif
#endif

/* Define to the equivalent of the C99 'restrict' keyword, or to
   nothing if this is not supported.  Do not define if restrict is
   supported directly.  */
#undef restrict
/* Work around a bug in Sun C++: it does not support _Restrict, even
   though the corresponding Sun C compiler does, which causes
   "#define restrict _Restrict" in the previous line.  Perhaps some future
   version of Sun C++ will work with _Restrict; if so, it'll probably
   define __RESTRICT, just as Sun C does.  */
#if defined __SUNPRO_CC && !defined __RESTRICT
# define _Restrict
#endif
