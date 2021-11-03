#ifndef CUSTOMER_ERRORS_H_INCLUDED
   #define CUSTOMER_ERRORS_H_INCLUDED

// Custom Errors for Cara- and ETS-Modules set with Windows-Function SetLastError().
// Request with GetLastError().

//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +---+-+-+-----------------------+-------------------------------+
//  |Sev|C|R|     Facility          |             Code              |
//  |   | | |   P   |      M        |                               |
//  +---+-+-+-----------------------+-------------------------------+
//      Sev - is the severity code
//      00 - Success, 01 - Informational, 10 - Warning, 11 - Error
//      C - is the Customer code flag
//      R - is a reserved bit
//  Facility
//      P - Product-Range 0 : ETS-Base-Module
//                        1 : Cara-Module
//                        2 : Cape-Module

#define CUSTOMER_CODE_FLAG          0x20000000
#define RESERVED_BIT                0x10000000

#define _SEVERITY_MASK               0xC0000000

#define _SEVERITY_SUCCESS            0x00000000
#define _SEVERITY_INFORMATIONAL      0x40000000
#define _SEVERITY_WARNING            0x80000000
#define _SEVERITY_ERROR              0xC0000000

#define PRODUCT_RANGE               0x0F000000

#define ETS_BASE_MODULE             0x00000000
#define PRODUCT_CARA_MODULE         0x01000000
#define PRODUCT_CAPE_MODULE         0x02000000

#define MODULE_RANGE                0x00FF0000

#define MODULE_COMMON               0x00010000
#define MODULE_ETSDIV               0x00020000
#define MODULE_ETS3DGL              0x00030000
#define MODULE_CARAWALK             0x00010000
#define MODULE_CARABOX              0x00020000
#define MODULE_CARASDB              0x00030000

#define CODE_RANGE                  0x0000FFFF
// Dll-Errors
#define DLL_LOAD_ERROR              0xE000FFFF  // Could not load Dll
#define FUNCTION_LOAD_ERROR         0xE0000000  // Could not load all functions

// Customer Errors for Math
#define MATHERR_DOMAIN              0x00000001  // domain
#define MATHERR_SING                0x00000002  // singularity
#define MATHERR_OVERFLOW            0x00000003  // overflow
#define MATHERR_UNDERFLOW           0x00000004  // underflow
#define MATHERR_PLOSS               0x00000005  // partial loss of significance
#define MATHERR_TLOSS               0x00000005  // total loss of significance
#define MATHERR_UNKNOWN             0x00000006  // unknown error
#define MATHERR_DENORMAL_OPERAND    0x00000007  // denormal operand
#define MATHERR_INVALID_OPERATION   0x00000008  // invalid operation
#define MATHERR_INEXACT_RESULT      0x00000009  // inexact result
#define MATHERR_STACK_CHECK         0x0000000a  // stack check
// Customer Errors
#define UNKNOWN_ERROR               0x0000000b  // Unknown
#define DESTRUCTION_ERROR           0x0000000c  // delete or free
#define ALLOCATION_ERROR            0x0000000d  // new or alloc
#define HEAP_ERROR                  0x0000000e  // heap
#define HEAP_DESTROY_ERROR          0x0000000f  // heap destroy
// Customer Errors for Fileacces
#define FILEERR_READ                0x00000011  // reading file
#define FILEERR_WRITE               0x00000012  // writing file
#define FILEERR_CRC                 0x00000013  // crc checksum
#define FILEERR_TYPE                0x00000014  // wrong file type
// Customer Errors for exceptions
#define ERR_INVALID_HANDLE          0x00000015  // invalid handle value
#define ERR_ACCESS_VIOLATION        0x00000016  // access violation
#define ERR_DATATYPE_MISALIGNMENT   0x00000017  // datatype misalignment
#define ERR_PRIV_INSTRUCTION        0x00000018  // priv instruction
#define ERR_IN_PAGE_ERROR           0x00000019  // in page_error
#define ERR_STACK_OVERFLOW          0x0000001a  // stack overflow
#define ERR_INVALID_DISPOSITION     0x0000001b  // invalid disposition
#define ERR_ARRAY_BOUNDS_EXCEEDED   0x0000001c  // array bounds exceeded
#define ERR_INT_DIVIDE_BY_ZERO      0x0000001d  // int divide by zero
#define ERR_INT_OVERFLOW            0x0000001e  // int overflow

// Customer Errors for CCaraDiv
#define ETSDIV_ERROR_DISTANCE       0xE0020021  // Distance out of Range
#define ETSDIV_ERROR_STEPS          0xE0020022  // Stepvalue out of Range
#define ETSDIV_ERROR_RANGE          0xE0020023  // wrong Range
#define ETSDIV_ERROR_STEPWIDTH      0xE0020024  // Stepwidthvalue out of range
#define ETSDIV_ERROR_NUMSTEPS       0xE0020025  // Number of steps out of Range
#define ETSDIV_ERROR_TEXTLEN        0xE0020026  // Textlen too small
#define ETSDIV_ERROR_MODE           0xE0020027  // wrong Numeric mode 
#define ETSDIV_ERROR_POINTER        0xE0020028  // Structure-pointer Zero
#define ETSDIV_ERROR_DIV_FKT        0xE0020029  // wrong Division-function
#define ETSDIV_ERROR_NO_ARRAY       0xE002002A  // No Array for Sin and Cos Values calculated

// Customer Errors for ETS3DGL
#define ETS3DERR_SET_HDC            0xE0030021  // Could not set HDC
#define ETS3DERR_SET_PIXFMT         0xE0030022  // Could not set pixelformat
#define ETS3DERR_CREATE_RC          0xE0030023  // Could not create rendercontext
#define ETS3DERR_MKCURR_RC          0xE0030024  // Could nat make the rendercontext current to DC
#define ETS3DERR_NOSYNCEVEVT        0xE0030025  // Could nat create a syncronise Event-Handle
#define ETS3DERR_NOTHREAD           0xE0030026  // Could not create a thread
#define ETS3DERR_TIMEOUT            0xE0030027  // Timeout waiting for OpenGL initializing
#define ETS3DERR_NOTIMER            0xE0030028  // Could not Create a Control-Timer
#define ETS3DERR_NOPALETTE          0xE0030029  // Kein Farbpaletten-Handle vorhanden

// Customer Errors for CCaraWalk
#define CARAWALK_UNKNOWN_ERROR      0xE1010000  // 
#define CARAWALK_DESTRUCTION_ERROR  0xE1010001  // 
#define CARAWALK_ALLOCATION_ERROR   0xE1010002  // 
#define CARAWALK_HEAP_ERROR         0xE1010003  // 
#define CARAWALK_ROOMDATA_ERROR     0xE1010021  // Could not set roomdata
#define CARAWALK_PARENTWND_ERROR    0xE1010022  // Invalid parent window
#define CARAWALK_BOXDATA_ERROR      0xE1010023  // Invalid box data

// Customer Errors for CCaraBox
#define CARABOX_UNKNOWN_ERROR       0xE1020000  // 
#define CARABOX_DESTRUCTION_ERROR   0xE1020001  // 
#define CARABOX_ALLOCATION_ERROR    0xE1020002  // 

// Customer Errors for CCaraSdB
#define CARASDB_UNKNOWN_ERROR       0xE1030000  // 
#define CARASDB_DESTRUCTION_ERROR   0xE1030001  // 
#define CARASDB_ALLOCATION_ERROR    0xE1030002  // 

#endif // CUSTOMER_ERRORS_H_INCLUDED
