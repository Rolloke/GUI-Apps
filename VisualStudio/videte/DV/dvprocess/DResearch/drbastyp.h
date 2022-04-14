/*------------------------------------------------------------------------*
 *
 * Basic type definitions that should be used in all DResearch sources!
 *
 * (C) 1996 - 1999 DResearch GmbH
 *
 * Maintained by Steffen Sledz (sledz@DResearch.DE)
 * Please send all suggestions, hints, extensions etc. to him.
 * Do not change anything by yourself!
 *
 * Do not use other macros/typedefs in your sources with the same
 * meanings like the macros/typedefs is this file!
 *
 * $Header: /DV/DVProcess/DResearch/drbastyp.h 1     15.02.01 11:59 Uwe $
 *------------------------------------------------------------------------*/

#ifndef __DRBASTYP_H__
#define __DRBASTYP_H__

/*---------------------------------------------------------------------*
 * platform dependent includes
 *---------------------------------------------------------------------*/
#if defined(__GNUC__)
# include <sys/types.h>
# include <limits.h>
#elif defined(__TCS__)
# include <sys/types.h>
# include <limits.h>
#elif (defined(AFX) || defined(_AFX)) && !defined(__AFX_H__)
# include <afx.h>
# include <limits.h>
#elif defined(__MSC__) || defined(_MSC_VER)
# if !defined(__STDC__)
#  include <wtypes.h>
# endif
# if defined(_WIN32_WCE)
#  include <types.h>
# else
#  include <sys/types.h>
# endif
# include <limits.h>
#else
#endif

/*---------------------------------------------------------------------*
 * the standard data types:
 * __u8, __s8, __u16, ...
 * (some often used aliases below)
 *---------------------------------------------------------------------*/
#if defined(_LINUX_TYPES_H)

#define HAS64 1

#elif defined(_WIN32_WCE)

typedef unsigned char         __u8;
typedef   signed char         __s8;
typedef unsigned short        __u16;
typedef   signed short        __s16;
typedef unsigned int          __u32;
typedef   signed int          __s32;
typedef unsigned __int64      __u64;
typedef   signed __int64      __s64;
#define HAS64 1

#elif defined(__MSC__) || defined(_MSC_VER)

typedef unsigned __int8       __u8;
typedef   signed __int8       __s8;
typedef unsigned __int16      __u16;
typedef   signed __int16      __s16;
typedef unsigned __int32      __u32;
typedef   signed __int32      __s32;
typedef unsigned __int64      __u64;
typedef   signed __int64      __s64;
#define HAS64 1

#else

typedef unsigned char         __u8;
typedef   signed char         __s8;
typedef unsigned short        __u16;
typedef   signed short        __s16;
typedef unsigned int          __u32;
typedef   signed int          __s32;

#if defined(DJGPP)
#define HAS64 1
typedef long long             __s64;
typedef unsigned long long    __u64;
#endif /* DJGPP */

#endif

#if !defined(HAS64)
#define HAS64 0
#endif

/* min/max values */

#define MIN_U8                0
#define MAX_U8                255
#define MIN_S8                (-128)
#define MAX_S8                127

#define MIN_U16               0
#define MAX_U16               65535
#define MIN_S16               (-32768)
#define MAX_S16               32767

#define MIN_U32               0
#define MAX_U32               4294967295
#define MIN_S32               (-2147483648)
#define MAX_S32               2147483647

#if HAS64
#define MIN_U64               0
#define MAX_U64               18446744073709551615
#define MIN_S64               (-9223372036854775808)
#define MAX_S64               9223372036854775807
#endif /* HAS64 */

/*---------------------------------------------------------------------*
 * some often used aliases below
 *---------------------------------------------------------------------*/

/* ----- unsigned 8 bit ----- (BYTE,UCHAR,UINT8,UBYTE,OCTET) */
#if !defined(_WINDOWS_) && !defined(__AFX_H__)
typedef __u8     BYTE;
#ifndef UCHAR
typedef __u8    UCHAR;
#endif /* !UCHAR */
#endif
#if !defined(_WIN32_WCE)
typedef __u8    UINT8;
#endif /* _WIN32_WCE */
typedef __u8    UBYTE;
typedef __u8    OCTET;
#ifndef MIN_BYTE
#define MIN_BYTE           MIN_U8
#endif
#ifndef MAX_BYTE
#define MAX_BYTE           MAX_U8
#endif
#ifndef MIN_UCHAR
#define MIN_UCHAR          MIN_U8
#endif
#ifndef MAX_UCHAR
#define MAX_UCHAR          MAX_U8
#endif
#ifndef MIN_UINT8
#define MIN_UINT8          MIN_U8
#endif
#ifndef MAX_UINT8
#define MAX_UINT8          MAX_U8
#endif
#ifndef MIN_UBYTE
#define MIN_UBYTE          MIN_U8
#endif
#ifndef MAX_UBYTE
#define MAX_UBYTE          MAX_U8
#endif
#ifndef MIN_OCTET
#define MIN_OCTET          MIN_U8
#endif
#ifndef MAX_OCTET
#define MAX_OCTET          MAX_U8
#endif

/* ----- signed 8 bit ----- (SBYTE,CHAR,INT8) */
typedef __s8    SBYTE;          

#if !(defined(_MSC_VER) && (_MSC_VER >= 1200))
#if !(defined(__cplusplus) && (defined(_WINDOWS_) || defined(__AFX_H__)))
typedef __s8    CHAR;
#endif
#endif
#if defined(_MSC_VER) && defined(__STDC__)
typedef __s8    CHAR;
#endif
#if !defined(_WIN32_WCE)
typedef __s8    INT8;           
#endif /* _WIN32_WCE */
#ifndef MIN_SBYTE
#define MIN_SBYTE          MIN_S8
#endif
#ifndef MAX_SBYTE
#define MAX_SBYTE          MAX_S8
#endif
#ifndef MIN_CHAR 
#define MIN_CHAR           MIN_S8
#endif
#ifndef MAX_CHAR 
#define MAX_CHAR           MAX_S8
#endif
#ifndef MIN_INT8 
#define MIN_INT8           MIN_S8
#endif
#ifndef MAX_INT8 
#define MAX_INT8           MAX_S8
#endif
                       
/* ----- unsigned 16 bit ----- (WORD,UINT16) */
#if !defined(_WINDOWS_) && !defined(__AFX_H__)
typedef __u16	 WORD;
#endif
#if !defined(_WIN32_WCE)
typedef __u16	 UINT16;
#endif /* _WIN32_WCE */
#ifndef MIN_WORD 
#define MIN_WORD           MIN_U16
#endif
#ifndef MAX_WORD 
#define MAX_WORD           MAX_U16
#endif
#ifndef MIN_UINT16 
#define MIN_UINT16         MIN_U16
#endif
#ifndef MAX_UINT16 
#define MAX_UINT16         MAX_U16
#endif

/* ----- signed 16 bit ----- (SWORD,INT16) */
typedef __s16	 SWORD;
#if !defined(_WIN32_WCE)
typedef __s16	 INT16;          
#endif /* _WIN32_WCE */
#ifndef MIN_SWORD 
#define MIN_SWORD          MIN_S16
#endif
#ifndef MAX_SWORD 
#define MAX_SWORD          MAX_S16
#endif
#ifndef MIN_INT16 
#define MIN_INT16          MIN_S16
#endif
#ifndef MAX_INT16 
#define MAX_INT16          MAX_S16
#endif
     	                
/* ----- unsigned 32 bit ----- (DWORD,UINT32) */
#if !defined(_WINDOWS_) && !defined(__AFX_H__) && !defined(_WIN32_WCE)
typedef __u32	 DWORD;
#endif
#if !(defined(_MSC_VER) && (_MSC_VER >= 1200))
typedef __u32	 UINT32;         
#endif
#if defined(_MSC_VER) && defined(__STDC__)
typedef __u32	 UINT32;         
#endif
#ifndef MIN_DWORD 
#define MIN_DWORD          MIN_U32
#endif
#ifndef MAX_DWORD 
#define MAX_DWORD          MAX_U32
#endif
#ifndef MIN_UINT32 
#define MIN_UINT32         MIN_U32
#endif
#ifndef MAX_UINT32 
#define MAX_UINT32         MAX_U32
#endif
     	                
/* ----- signed 32 bit ----- (SDWORD,LONG,INT32) */
typedef __s32	 SDWORD;          
#if !defined(_WINDOWS_) && !defined(__AFX_H__) && !defined(_WIN32_WCE)
typedef __s32	 LONG;
#endif
#if !(defined(_MSC_VER) && (_MSC_VER >= 1200))
typedef __s32	 INT32;          
#endif
#if defined(_MSC_VER) && defined(__STDC__)
typedef __s32	 INT32;          
#endif
#ifndef MIN_SDWORD 
#define MIN_SDWORD         MIN_S32
#endif
#ifndef MAX_SDWORD 
#define MAX_SDWORD         MAX_S32
#endif
#ifndef MIN_LONG 
#define MIN_LONG           MIN_S32
#endif
#ifndef MAX_LONG 
#define MAX_LONG           MAX_S32
#endif
#ifndef MIN_INT32 
#define MIN_INT32          MIN_S32
#endif
#ifndef MAX_INT32 
#define MAX_INT32          MAX_S32
#endif
     	                
#if HAS64              
/* ----- unsigned 64 bit ----- (QWORD,DWORDLONG,UINT64) */
typedef __u64	 QWORD;
#if !defined(_WINDOWS_) && !defined(__AFX_H__)
typedef __u64	 DWORDLONG;
#endif
#if !defined(_WIN32_WCE)
typedef __u64	 UINT64;
#endif /* _WIN32_WCE */
#ifndef MIN_QWORD 
#define MIN_QWORD          MIN_U64
#endif
#ifndef MAX_QWORD 
#define MAX_QWORD          MAX_U64
#endif
#ifndef MIN_DWORDLONG 
#define MIN_DWORDLONG      MIN_U64
#endif
#ifndef MAX_DWORDLONG 
#define MAX_DWORDLONG      MAX_U64
#endif
#ifndef MIN_UINT64 
#define MIN_UINT64         MIN_U64
#endif
#ifndef MAX_UINT64 
#define MAX_UINT64         MAX_U64
#endif

/* ----- signed 64 bit ----- (LONGLONG,INT64) */
#if !defined(_WINDOWS_) && !defined(__AFX_H__)
typedef __s64	 LONGLONG;
#endif
#if !defined(_WIN32_WCE)
typedef __s64	 INT64;
#endif /* _WIN32_WCE */
#ifndef MIN_LONGLONG 
#define MIN_LONGLONG       MIN_S64
#endif
#ifndef MAX_LONGLONG 
#define MAX_LONGLONG       MAX_S64
#endif
#ifndef MIN_INT64 
#define MIN_INT64          MIN_S64
#endif
#ifndef MAX_INT64 
#define MAX_INT64          MAX_S64
#endif
#endif /* HAS64 */

/*---------------------------------------------------------------------*
 * BOOL
 *---------------------------------------------------------------------*/

#if !defined(_WINDOWS_) && !defined(__AFX_H__)

#if defined(__cplusplus)
typedef bool BOOL;
#define FALSE  false
#define TRUE   true
#else /* not __cplusplus */
typedef int BOOL;
#define FALSE  0
#define TRUE   1
#endif /* not __cplusplus */

#endif

/*---------------------------------------------------------------------*
 * INLINE    ...  inline in release version only
 * __INLINE  ...  inline in debug version too
 *---------------------------------------------------------------------*/

#if defined(__cplusplus)

#if defined(DEBUG)
# define INLINE       static
# define __INLINE     inline
#else
# define INLINE       inline
# define __INLINE     inline
#endif

#else /* not __cpluscplus */

#if defined(DEBUG)
# if defined(__GNUC__)
#  define INLINE       static
#  define __INLINE     __inline
# elif defined(__MSC__) || defined(_MSC_VER)
#  define INLINE       static
#  define __INLINE     __inline
# elif defined(__TCS_V2__)
#  define INLINE       static
#  define __INLINE     inline
# else
#  define INLINE       static
#  define __INLINE     static
# endif
#else /* not DEBUG */
# if defined(__GNUC__)
#  define INLINE       __inline
#  define __INLINE     __inline
# elif defined(__MSC__) || defined(_MSC_VER)
#  define INLINE       __inline
#  define __INLINE     __inline
# elif defined(__TCS_V2__)
#  define INLINE       inline
#  define __INLINE     inline
# else
#  define INLINE       static
#  define __INLINE     static
# endif
#endif /* not DEBUG */

#endif

/*---------------------------------------------------------------------*
 * CONST
 *---------------------------------------------------------------------*/

#ifndef CONST
#define CONST const  /* else empty */
#endif

/*---------------------------------------------------------------------*
 * RESTRICT
 *---------------------------------------------------------------------*/

#ifndef RESTRICT

#if defined(__TCS__) || defined(__ICL)
# define RESTRICT restrict  /* else empty */
#else
# define RESTRICT
#endif

#endif

/*---------------------------------------------------------------------*
 * NULL
 *---------------------------------------------------------------------*/

#ifndef NULL
#if defined(__cplusplus)
#define NULL 0
#else
#define NULL  ((void*)(0))
#endif
#endif

/*---------------------------------------------------------------------*
 * other
 *---------------------------------------------------------------------*/

/* some compilers don't know about __cdecl stuff */
#if defined(DJGPP) || defined(__TCS__) || defined(__GNUC__)
#ifndef __cdecl
#define __cdecl
#endif
#endif

/*---------------------------------------------------------------------*/
#endif /* __DRBASTYP_H__ */

