/*
*******************************************************************************
                       Copyright eMuzed Inc., 2001-2002.
All rights Reserved, Licensed Software Confidential and Proprietary Information 
    of eMuzed Incorporation Made available under Non-Disclosure Agreement OR
                            License as applicable.
*******************************************************************************
*/

/*
*******************************************************************************
Product     : Common module 
Module      : 
File        : common.h
Description : This is the commom header to be used by all modules.

Revision Record:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Date            Id          Author          Comment 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Jan 31, 2001                Satish Nayak    Initial code
*******************************************************************************
*/

#ifndef INCLUDE_COMMON
#define INCLUDE_COMMON


typedef  char          		  int8;
typedef unsigned char        uint8;
typedef short int            int16;
typedef unsigned short int   uint16;
typedef int                  int32;
typedef unsigned int         uint32;

typedef float                flt32;
typedef double               flt64;

typedef unsigned char        tBool;
typedef signed int           tError;




#define E_SUCCESS            0
#define E_FAILURE           (-1)
#define E_OUT_OF_MEMORY     (-2)

#define E_ON                 1
#define E_OFF                0



#endif  /* INCLUDE_COMMON */

