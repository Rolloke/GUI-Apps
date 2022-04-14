/****************************************************************************/
/**                                                                        **/
/**                              Hardlock                                  **/
/**                    API-Structures and definitions                      **/
/**                                                                        **/
/**   This file contains some helpful defines to access a Hardlock using   **/
/**   the application programing interface (API) for Hardlock.             **/
/**                                                                        **/
/**              ///FAST Software Security - Group Aladdin                 **/
/**                                                                        **/
/**  Revision history                                                      **/
/**  ----------------                                                      **/
/**  (related to API version)                                              **/
/**  3.18  -- Define HIGH_DLL changed to HLHIGH_DLL (Watcom 9.5 doesn't    **/
/**           like my define)                                              **/
/**        -- New function call API_GET_TASKID                             **/
/**        -- HLXLOGIN no longer needed.                                   **/
/**        -- Add UNIX features (M_UNIX defined)                           **/
/**        -- Add Windows NT feature (WINNT defined)                       **/
/**  3.19  -- ifdef for Symantec C included (DOS386)                       **/
/**  3.20  -- ifdef for WIN32s included                                    **/
/**           ifdef for Intel Codebuilder removed (compiler to buggy)      **/
/**  3.22  -- ifdef for Zortech included                                   **/
/**        -- combined OS/2 and DOS include files                          **/
/**  3.23  -- PortFlags added to API structure.                            **/
/**  3.24  -- defined CALL_ for 16bit Watcom                               **/
/**  3.25  -- Different includes merged.                                   **/
/**                                                                        **/
/**  (related to VCS version)
***  $Log: /Project/CIPC/Fastapi.h $
 * 
 * 3     15.07.98 10:21 Hedu
 * 
 * 2     20.08.97 17:49 Hedu
 * 
 * 1     20.08.97 17:48 Hedu
***  Revision 1.11  1997/02/03 18:08:36  henri
***  Renamed error 17
***  
***  Revision 1.10  1997/01/30 17:16:55  henri
***  Added LM return codes.
***  
***  Revision 1.9  1997/01/28 08:23:30  henri
***  Missed a semicolon ;-)
***  
***  Revision 1.8  1997/01/27 17:57:11  henri
***  Added slot number in API structure.
***  
***  Revision 1.7  1997/01/16 18:18:11  henri
***  Added API_LMINIT function code.
***
***  Revision 1.6  1996/11/13 16:55:49  chris
***  added SOLARIS & UNIX32 define
***
***  Revision 1.5  1996/08/12 16:23:43  henri
***  Added VCS log.
***
**/
/****************************************************************************/

#if !defined(_FASTAPI_H_)
#define _FASTAPI_H_

/* HEDU test */
#define WINNT

#ifdef LINUX
  #define UNIX32
#endif

#ifdef SOLARIS
  #define UNIX32
#endif

#ifdef SCO
  #define UNIX32
#endif

#ifdef __OS2__
  #ifdef INTERNAL_16BITDLL
    #define LOAD_DS
  #else
    #ifdef __WATCOMC__
      #ifdef __386__      /* not the 16bit compiler */
        #include <os2.h>
      #endif
    #else
      #include <os2.h>
    #endif
  #endif
  #ifdef OS_16
    #define RET_        Word
    #define FAR_        far pascal
    #define DATAFAR_    far
  #else
    #define RET_        APIRET
    #define FAR_
    #define CALL_       APIENTRY
    #define DATAFAR_
  #endif
  #pragma pack(2)
#endif

#ifdef UNIX32
  #define __386__
  #define pascal
  #pragma pack(1)
#endif

#ifdef __GNUC__
  #define __386__
  #define pascal
  #pragma pack(1)
#endif

#ifdef __WIN32__
  #define __386__
#endif

#ifdef WINNT
  #ifndef __386__       /* Watcom doesnt like it */
    #define __386__
  #endif
  #pragma pack(1)
  #ifdef DLL
    #define CALL_ __stdcall
  #else
    #define CALL_
  #endif
#endif

#ifdef DOS386           /* Symantec C            */
  #define __386__
  #pragma pack(2)
#endif

#ifdef __HIGHC__        /* Metaware High C       */
  #define __386__
  #define _PACKED _Packed
#endif

#ifdef __ZTC__          /* Zortech C             */
  #define __386__
#endif

#ifdef SALFORD          /* Salford C             */
  #define ALIGN_ 8
#endif

#ifdef __WATCOMC__
  #ifndef __386__
    #ifndef OS_16
      #define CALL_ cdecl
    #endif
  #endif
#endif

#ifdef _CVI_            /* LabWindows/CVI        */
  #define RET_     Word
  #define FAR_     far
  #define CALL_    pascal
  #define DATAFAR_ far
#endif

#ifdef __386__
  #define DATAFAR_
  #define FAR_
#endif

#ifdef HLHIGH_DLL
  #define CALL_ pascal _export
#endif

#ifdef LOAD_DS
  #define CALL_ _loadds
#endif

#ifndef CALL_
  #define CALL_
#endif

#ifndef _PACKED
  #define _PACKED
#endif

/* <HEDU> bold attack just set as it should be, duuno the available compiler flags*/
#define DATAFAR_ 
#define FAR_
#define ALIGN_
/* </HEDU> */

#ifndef DATAFAR_
	#define DATAFAR_ far
#endif

#ifndef FAR_
  #define FAR_ far
#endif

#ifndef RET_
  #define RET_ Word
#endif

#ifndef ALIGN_
  #define ALIGN_
#endif

/* -------------------------------- */
/* Definitions and API structures : */
/* -------------------------------- */
typedef unsigned char  Byte;
typedef unsigned short Word;
typedef unsigned long  Long;

typedef struct
  {
  Word Use_Key;
  Byte Key[8];
  } DES_MODE;

typedef struct
  {
  Word ModAd;                           /* Hardlock module address */
  Word Reg;                             /* Memory register adress  */
  Word Value;                           /* Memory value            */
  Byte Reserved[4];
  } EYE_MODE;

typedef struct
  {
  Word LT_Reserved;
  Word Reg;                             /* Memory register adress       */
  Word Value;                           /* Memory value                 */
  Word Password[2];                     /* Access passwords             */
  } LT_MODE;

typedef union
  {
  DES_MODE Des;
  EYE_MODE Eye;
  LT_MODE  Lt;
  } HARDWARE;

typedef _PACKED struct ALIGN_ hl_api
  {
  Byte          API_Version_ID[2];      /* Version                    */
  Word          API_Options[2];         /* API Optionflags            */
  Word          ModID;                  /* Modul-ID (EYE = 0...)      */
  HARDWARE      Module;                 /* Hardware type              */

#ifdef __OS2__                          /* Pointer to cipher data     */
  #ifdef OS_16
    void far *Data;
   #else
    #ifdef __BORLANDC__
      void FAR16PTR Data;
    #else
      void * _Seg16 Data;
    #endif
  #endif
 #else
  void DATAFAR_ *Data;
#endif

  Word          Bcnt;                   /* Number of blocks            */
  Word          Function;               /* Function number             */
  Word          Status;                 /* Actual status               */
  Word          Remote;                 /* Remote or local??           */
  Word          Port;                   /* Port address if local       */
  Word          Speed;                  /* Speed of port if local      */
  Word          NetUsers;               /* Current Logins (HL-Server)  */
  Byte          ID_Ref[8];              /* Referencestring             */
  Byte          ID_Verify[8];           /* Encrypted ID_Ref            */
  Long          Task_ID;                /* Multitasking program ID     */
  Word          MaxUsers;               /* Maximum Logins (HL-Server)  */
  Long          Timeout;                /* Login Timeout in minutes    */
  Word          ShortLife;              /* (multiple use)              */
  Word          Application;            /* Application number          */
  Word          Protocol;               /* Protocol flags              */
  Word          PM_Host;                /* DOS Extender type           */
  Long          OSspecific;             /* ptr to OS specific data     */
  Word          PortMask;               /* Default local search (in)   */
  Word          PortFlags;              /* Default local search (out)  */
  Word          EnvMask;                /* Use env string search (in)  */
  Word          EnvFlags;               /* Use env string search (out) */
  Word          Reserved1[2];           /* Reserved area               */
  Word          Slot_ID;                /* Licence slot number         */
  Byte          Reserved2[168];         /* Reserved area               */
  } HL_API, LT_API;

#ifdef M_UNIX
  #pragma pack()
#endif

#ifdef __OS2__
  #pragma pack()
#endif

/* ------------- */
/* Module-ID's : */
/* ------------- */
#define EYE_DONGLE       0              /* Hardlock E-Y-E             */
#define DES_DONGLE       1              /* FAST DES                   */
#define LT_DONGLE        3              /* Hardlock LT                */

/* --------------------- */
/* API function calls  : */
/* --------------------- */
#define API_INIT            0           /* Init API structure          */
#define API_DOWN            1           /* Free API structure          */
#define API_FORCE_DOWN      31          /* Force deinintialization     */
#define API_MULTI_SHELL_ON  2           /* MTS is enabled              */
#define API_MULTI_SHELL_OFF 3           /* MTS is disabled             */
#define API_MULTI_ON        4           /* Enable MTS                  */
#define API_MULTI_OFF       5           /* Disable  MTS                */
#define API_AVAIL           6           /* Dongle available?           */
#define API_LOGIN           7           /* Login dongle server         */
#define API_LOGOUT          8           /* Logout dongle server        */
#define API_INFO            9           /* Get API informations        */
#define API_GET_TASKID      32          /* Get TaskID from API         */
#define API_LOGIN_INFO      34          /* Get API Login informations  */

/* --------------------------- */
/* Data and memory functions : */
/* --------------------------- */
#define API_KEYE             11         /* Use KEYE for encryption         */
#define API_READ             20         /* Read one word of dongle EEPROM  */
#define API_WRITE            21         /* Write one word of dongle EEPROM */
#define API_READ_BLOCK       23         /* Read EEPROM in one block        */
#define API_WRITE_BLOCK      24         /* Write EEPROM in one block       */
#define API_ABORT            51         /* Critical Error Abort            */

/* -------------- */
/* LM functions : */
/* -------------- */
#define API_LMINIT           40         /* LM compatible API_INIT replacement           */
#define API_LMPING           41         /* checks if LM dongle and slot is available    */
#define API_LMINFO           42         /* info about currently used LIMA               */

/* -------------------- */
/* Dongle access mode : */
/* -------------------- */
#define LOCAL_DEVICE    1               /* Query local HL only         */
#define NET_DEVICE      2               /* Query remote HL only        */
#define DONT_CARE       3               /* Query local or remote HL    */

/* --------------- */
/* EnvMask flags : */
/* --------------- */
#define IGNORE_ENVIRONMENT 0x8000       /* If set the environment is   */
                                        /* not scaned for HL_SEARCH    */
/* ------------------ */
/* API PM_Host ID's : */
/* ------------------ */
#define API_XTD_DETECT    0
#define API_XTD_DPMI      1             /* QDPMI, Borland, Windows ... */
#define API_XTD_PHAR386   2
#define API_XTD_PHAR286   3
#define API_XTD_CODEBLDR  4             /* Intel Code Builder          */
#define API_XTD_COBOLXM   5

/* ------------------ */
/* API Status Codes : */
/* ------------------ */
#define STATUS_OK                 0     /* API call was succesfull        */
#define NOT_INIT                  1     /* DONGLE not initialized         */
#define ALREADY_INIT              2     /* Already initialized            */
#define UNKNOWN_DONGLE            3     /* Device not supported           */
#define UNKNOWN_FUNCTION          4     /* Function not supported         */
#define HLS_FULL                  6     /* HL-Server login table full     */
#define NO_DONGLE                 7     /* No device available            */
#define NETWORK_ERROR             8     /* A network error occured        */
#define NO_ACCESS                 9     /* No device available            */
#define INVALID_PARAM            10     /* A wrong parameter occured      */
#define VERSION_MISMATCH         11     /* HL-Server not API version      */
#define DOS_ALLOC_ERROR          12     /* Error on memory allocation     */
#define CANNOT_OPEN_DRIVER       14     /* Can not open driver (NT,UNIX)  */
#define INVALID_ENV              15     /* Invalid environment string     */
#define DYNALINK_FAILED          16     /* Unable to get a function entry */
#define INVALID_LIC              17     /* No valid licence info (LM)     */
#define NO_LICENSE               18     /* Slot/licence not enabled (LM)  */
#define TOO_MANY_USERS          256     /* Login table full (remote)      */
#define SELECT_DOWN             257     /* Printer not On-line            */

#endif /*_FASTAPI_H_*/
/* eof */

