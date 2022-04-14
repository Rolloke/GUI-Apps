/****************************************************************************/
/**                                                                        **/
/**                           Hardlock API-Calls                           **/
/**                                                                        **/
/**   This program is just an example for some functions to access the     **/
/**   application programing interface (API) for Hardlock. Feel free to    **/
/**   define your own functions.                                           **/
/**                                                                        **/
/**                ///FAST Software Security - Group Aladdin               **/
/**                                                                        **/
/**              Computer: IBM PC or compatible                            **/
/**                   OS : MS-PC/DOS 2.0 or higher and MS Windows 3.x      **/
/**              Language: C                                               **/
/**                                                                        **/
/**  Revision history                                                      **/
/**  ----------------                                                      **/
/**  (related to API version)                                              **/
/**  3.18  -- Get the TaskID from API function.                            **/
/**        -- HL_MEMINF strategy changed                                   **/
/**        -- New function: HL_HLSVERS: return HL-Server version           **/
/**  3.19  -- ifdef for Symantec C included (DOS386)                       **/
/**        -- CENTRY-DLL_USE combination (for Windows NT/32s) by Chris     **/
/**  3.22  -- Search order added to HL_LOGIN, call to function (33).       **/
/**  3.23  -- Search order removed, handeled internal by LowAPI.           **/
/**        -- HL_LOGOUT, HL_LOGIN optimized (call API_LOGIN and API_LOGOUT **/
/**           only for remote access).                                     **/
/**        -- Environment search string not yet supported for FlashTek.    **/
/**           Must be compiled with /dNO_ENVCHK                            **/
/**        -- release                                                      **/
/**  3.24  -- API_Function defined                                         **/
/**  3.25  -- Bugfix for HL-Server licenced to one user.                   **/
/**        -- No further checks on HL_LOGIN on specific errors.            **/
/**                                                                        **/
/**  (related to VCS version)
***  $Log: /Project/CIPC/Hlapi_c.c $
 * 
 * 5     17.09.98 10:46 Hedu
 * 
 * 4     14.09.98 13:45 Hedu
 * 
 * 3     15.07.98 10:21 Hedu
 * 
 * 2     20.08.97 11:46 Hedu
 * 
 * 1     19.08.97 15:11 Hedu
 * 
 * 4     19.08.97 15:07 Hedu
 * 
 * 3     18.08.97 20:12 Hedu
 * 
 * 2     18.08.97 19:27 Hedu
 * 
 * 1     18.08.97 18:13 Hedu
***  Revision 1.12  1997/02/12 13:44:46  henri
***  Changed return code handling for LM.
***  
***  Revision 1.11  1997/02/11 20:16:34  chris
***  High-API now checks whether Low-API supports API_LMINIT function.
***  
***  Revision 1.10  1997/02/11 18:32:29  chris
***  fixed error code returned in HL_LMLOGIN()
***  
***  Revision 1.9  1997/02/06 14:12:58  henri
***  Added one character to sslen.
***  
***  Revision 1.8  1997/02/03 18:10:01  henri
***  Renamed INVALID_MEM -> INVALID_LIC
***  
***  Revision 1.7  1997/01/31 17:37:31  henri
***  Added first version of HL_LMLOGIN
***  
***  Revision 1.6  1996/11/13 16:56:25  chris
***  adapted to UNIX32 define
***
***  Revision 1.5  1996/10/02 20:34:09  HENRI
***  Added GETSTRUC define because of wrong pointer init by old Power Fortran linker.
***
***  Revision 1.4  1996/09/18 11:52:14  henri
***  Support for multi API strutures.
***
***  Revision 1.3  1996/08/08 14:47:01  henri
***  Added VCS log.
***
**/
/****************************************************************************/

#ifdef  _MSC_VER
#pragma warning (disable:4103)                /* no #pragma pack warnings */
#pragma warning (disable:4711)  /* HEDU no automatic inline warning */
#pragma warning (disable:4244)  /* HEDU possible los of data */
#endif

#define GETSTRUC if (!api_struc) api_struc = &hl_struc

#include "hlapi_c.h"

static HL_API hl_struc = {{0}};                 /* Hardlock API structure   */
static HL_API DATAFAR_ *api_struc=0;            /* Pointer to API structure */

static Word CALL_API (HL_API DATAFAR_ *api_struc);

#ifdef __cplusplus
  extern "C" {
#endif

#ifdef UNIX32
  #define CENTRY
#endif

#ifdef WINNT
  #define CENTRY
  #ifdef __WATCOMC__
    #pragma aux cdecl "_*" parm caller [] value struct routine [eax] modify [eax ebx ecx edx]
    #pragma aux (cdecl) API_HL_CSTACK;
  #endif
#endif

#ifdef SALFORD
  #include <dbos\lib.h>
  #define CENTRY
#endif

#ifdef CENTRY
  #ifdef DLL_USE
    #ifdef WINNT
      extern Word __stdcall _API_DLL(HL_API DATAFAR_ *api_struc);
      #define API_Function _API_DLL
    #endif
  #else
    #ifdef WINNT
      extern Word FAR_ __cdecl API_HL_CSTACK(HL_API DATAFAR_ *api_struc);
    #else
      extern Word FAR_ API_HL_CSTACK(HL_API DATAFAR_ *api_struc);
    #endif
    #define API_Function API_HL_CSTACK
  #endif
#else
  #ifdef DLL_USE
    extern Word FAR_ pascal _API_DLL(HL_API DATAFAR_ *api_struc);
    #define API_Function _API_DLL
  #else
    #ifdef __HIGHC__
      extern Word _API_HL_CSTACK(HL_API DATAFAR_ *api_struc);
      #define API_Function _API_HL_CSTACK
    #else
      extern Word FAR_ pascal _API_HL_STACK(HL_API DATAFAR_ *api_struc);
	  #define API_Function _API_HL_STACK
    #endif
  #endif
#endif

#ifdef __cplusplus
  }
#endif

/****************************************************************************/
static Word CALL_API(HL_API DATAFAR_ *api_struc)
/****************************************************************************/
{
Word result;

/* ----------------- */
/* We call the API : */
/* ----------------- */

#ifdef SALFORD
  set_io_permission(1);
#endif

result = API_Function(api_struc);

#ifdef SALFORD
  set_io_permission(0);
#endif

return result;
}

/****************************************************************************/
RET_ FAR_ CALL_ HL_SELECT(HL_API DATAFAR_ *hl_ptr)
/****************************************************************************/
{
if (!hl_ptr)
  api_struc = &hl_struc;
 else
  api_struc = hl_ptr;

return (STATUS_OK);
}

/****************************************************************************/
RET_ FAR_ CALL_ HL_LOGOUT(void)
/****************************************************************************/
{
GETSTRUC;
if (api_struc->Remote == NET_DEVICE)
  {
  api_struc->Function = API_LOGOUT;
  CALL_API(api_struc);
  }

api_struc->Function = API_DOWN;
CALL_API(api_struc);
if (api_struc->Status)
  {
  api_struc->Function = API_FORCE_DOWN;
  CALL_API(api_struc);
  }

api_struc->Task_ID = 0;
return (api_struc->Status);
}

/****************************************************************************/
RET_ FAR_ CALL_ HL_PORTINF(void)
/****************************************************************************/
{
GETSTRUC;
api_struc->Function = API_AVAIL;
CALL_API(api_struc);

if (api_struc->Status) return (0xffff);
return (api_struc->Port);
}

/****************************************************************************/
RET_ FAR_ CALL_ HL_USERINF(void)
/****************************************************************************/
{
GETSTRUC;

if (api_struc->Slot_ID == 0)
  api_struc->Function = API_INFO;
 else
  api_struc->Function = API_AVAIL;

api_struc->Application = 0;
CALL_API(api_struc);

if (api_struc->Status) return (0xffff);
return (api_struc->NetUsers);
}

/****************************************************************************/
RET_ FAR_ CALL_ HL_MAXUSER(void)
/****************************************************************************/
{
GETSTRUC;

if (api_struc->Slot_ID == 0)
  api_struc->Function = API_INFO;
 else
  api_struc->Function = API_AVAIL;

CALL_API(api_struc);

if (api_struc->Status) return (0xffff);
return (api_struc->MaxUsers);
}

/****************************************************************************/
RET_ FAR_ CALL_ HL_ACCINF(void)
/****************************************************************************/
{
GETSTRUC;
api_struc->Function = API_AVAIL;
CALL_API(api_struc);

if (api_struc->Status) return (0xffff);
return (api_struc->Remote);
}

/****************************************************************************/
RET_ FAR_ CALL_ HL_VERSION(void)
/****************************************************************************/
{
GETSTRUC;
api_struc->Function = API_AVAIL;
CALL_API(api_struc);

if (api_struc->Status) return (0xffff);
return ((api_struc->API_Version_ID[0] * 100) + api_struc->API_Version_ID[1]);
}

/****************************************************************************/
RET_ FAR_ CALL_ HL_HLSVERS(void)
/****************************************************************************/
{
Word result;

GETSTRUC;
if (api_struc->Remote == NET_DEVICE)
  {
  api_struc->Function  = API_AVAIL;
  result               = CALL_API(api_struc);
  if (result == STATUS_OK)
    return (api_struc->ShortLife);
  }
return 0;
}

/****************************************************************************/
RET_ FAR_ CALL_ HL_AVAIL(void)
/****************************************************************************/
{
GETSTRUC;
api_struc->Function = API_AVAIL;
CALL_API(api_struc);

if ((api_struc->Status == NO_DONGLE) && (api_struc->ShortLife == 1) && (api_struc->Remote == LOCAL_DEVICE))
  return(SELECT_DOWN);

return (api_struc->Status);
}

/****************************************************************************/
RET_ FAR_ CALL_ HL_CODE(void DATAFAR_ *Data, Word Cnt)
/****************************************************************************/
{
GETSTRUC;
api_struc->Data     = Data;
api_struc->Bcnt     = Cnt;
api_struc->Function = API_KEYE;
CALL_API(api_struc);
api_struc->Bcnt     = 0;

return (api_struc->Status);
}

/****************************************************************************/
RET_ FAR_ CALL_ HL_LOGIN(Word ModAd, Word Access, Byte DATAFAR_ * RefKey, Byte DATAFAR_ * VerKey)
/****************************************************************************/
{
Word  result;
Long  OldTimerID;
Word  OldRemote, OldPort;
short i;

GETSTRUC;
OldRemote  = api_struc->Remote;
OldTimerID = api_struc->Task_ID;
OldPort    = api_struc->Port;

api_struc->ModID            = EYE_DONGLE;
api_struc->Remote           = Access;
api_struc->Module.Eye.ModAd = ModAd;

/* --------------------- */
/* Get TaskID from API : */
/* --------------------- */
#ifndef DOS386
if (api_struc->Task_ID == 0)
  {
  api_struc->Function = API_GET_TASKID;
  CALL_API(api_struc);
  }
#endif

api_struc->Protocol = 0;
api_struc->Port     = 0;
api_struc->Timeout  = 0;
api_struc->PM_Host  = API_XTD_DETECT;

/* -------------------------------------------- */
/* We generated a verify key with TESTAPI.EXE : */
/* -------------------------------------------- */
for (i = 0; i < 8; i++)
  {
  api_struc->ID_Ref[i]    = RefKey[i];
  api_struc->ID_Verify[i] = VerKey[i];
  }

/* ------------------------- */
/* Don't check for HL_SEARCH */
/* ------------------------- */
#ifdef NO_ENVCHK
api_struc->EnvMask |= IGNORE_ENVIRONMENT;
#endif

/* --------------------------------------------- */
/* Call the INIT (search for Hardlock) function :*/
/* --------------------------------------------- */
api_struc->Function = API_INIT;
result             = CALL_API(api_struc);
if (result == ALREADY_INIT)
  {
  api_struc->Remote  = OldRemote;
  api_struc->Task_ID = OldTimerID;
  api_struc->Port    = OldPort;
  return (result);
  }

/* ----------------------------- */
/* No further checks necessary : */
/* ----------------------------- */
if ((result == INVALID_ENV) || (result == CANNOT_OPEN_DRIVER))
  return (result);

/* ----------------------------- */
/* Check, if HL-Server is full : */
/* ----------------------------- */
if (result != STATUS_OK)
  {
  if ((Access & NET_DEVICE) == NET_DEVICE)
    {
    api_struc->Function = API_LOGIN_INFO;
    if (CALL_API(api_struc) == HLS_FULL)
      result = TOO_MANY_USERS;
    }
  return (result);
  }

/* ------------------------------------------ */
/* Login to HL-Server if Hardlock is remote : */
/* ------------------------------------------ */
if (api_struc->Remote == NET_DEVICE)
  {
  api_struc->Function = API_LOGIN;
  result              = CALL_API(api_struc);
  if(result)
    {
    api_struc->Function = API_DOWN;
    CALL_API(api_struc);
    }
  }

return result;
}


/****************************************************************************/
RET_ FAR_ CALL_ HL_READ(Word Reg, Word DATAFAR_ *Value)
/****************************************************************************/
{
GETSTRUC;
if(Reg <= 63)
  {
  api_struc->Module.Eye.Reg = Reg;
  api_struc->Function       = API_READ;
  CALL_API(api_struc);
  if(api_struc->Status) return (api_struc->Status);
   *Value = api_struc->Module.Eye.Value;
  return (STATUS_OK);
  }

return (INVALID_PARAM);
}

/****************************************************************************/
RET_ FAR_ CALL_ HL_READBL(Byte DATAFAR_ * Eeprom)
/****************************************************************************/
{
GETSTRUC;
api_struc->Bcnt     = 16;
api_struc->Data     = Eeprom;
api_struc->Function = API_READ_BLOCK;
CALL_API(api_struc);
api_struc->Bcnt     = 0;

return (api_struc->Status);
}

/****************************************************************************/
RET_ FAR_ CALL_ HL_WRITEBL(Byte DATAFAR_ * Eeprom)
/****************************************************************************/
{
GETSTRUC;
api_struc->Bcnt     = 4;
api_struc->Data     = Eeprom;
api_struc->Function = API_WRITE_BLOCK;
CALL_API(api_struc);
api_struc->Bcnt     = 0;

return (api_struc->Status);
}

/****************************************************************************/
RET_ FAR_ CALL_ HL_ABORT(void)
/****************************************************************************/
{
GETSTRUC;
api_struc->Function = API_LOGOUT;
CALL_API(api_struc);
api_struc->Function = API_ABORT;
CALL_API(api_struc);

return (api_struc->Status);
}

/****************************************************************************/
RET_ FAR_ CALL_ HL_WRITE(Word Reg, Word Value)
/****************************************************************************/
{
GETSTRUC;
if(Reg >= 48 && Reg <= 63)
  {
  api_struc->Module.Eye.Reg   = Reg;
  api_struc->Module.Eye.Value = Value;
  api_struc->Function         = API_WRITE;
  CALL_API(api_struc);
  return (api_struc->Status);
  }

return (INVALID_PARAM);
}

/****************************************************************************/
RET_ FAR_ CALL_ HL_MEMINF(void)
/****************************************************************************/
{
Word newvalue, oldvalue;
Word TestMem = 0;
Byte Memory[128];
int i;

/* -------------------------- */
/* Read memory in one block : */
/* -------------------------- */
GETSTRUC;
api_struc->Bcnt     = 16;
api_struc->Data     = Memory;
api_struc->Function = API_READ_BLOCK;
CALL_API(api_struc);
api_struc->Bcnt     = 0;
if (api_struc->Status != STATUS_OK)
  return (NO_ACCESS);

/* -------------------------------------- */
/* Check, if every value is zero or one : */
/* -------------------------------------- */
for (i = 0; i < 128; i++)
  TestMem |= Memory[i];
if (TestMem != 0)
  {
  for (i = 0; i < 128; i++)
    {
    if (Memory[i] != 0xff)
      return (STATUS_OK);
    }
  }

/* ---------------------- */
/* Save memory contents : */
/* ---------------------- */
api_struc->Module.Eye.Reg = 48;
api_struc->Function       = API_READ;
CALL_API(api_struc);
if(api_struc->Status) return (NO_ACCESS);
oldvalue                 = api_struc->Module.Eye.Value;

/* ---------------------------------------------------------------- */
/* XOR of the read value to exclude random returns from interface : */
/* ---------------------------------------------------------------- */
newvalue = oldvalue ^ 0x0d0e;

/* ------------------------ */
/* Write new memory value : */
/* ------------------------ */
api_struc->Module.Eye.Value = newvalue;
api_struc->Function         = API_WRITE;
CALL_API(api_struc);
if(api_struc->Status)
 {
 api_struc->Module.Eye.Value = oldvalue;
 api_struc->Function         = API_WRITE;
 CALL_API(api_struc);
 return(NO_ACCESS);
 }

/* ------------------------- */
/* Read and compare memory : */
/* ------------------------- */
api_struc->Function = API_READ;
CALL_API(api_struc);
if(api_struc->Status)
 {
 api_struc->Module.Eye.Value = oldvalue;
 api_struc->Function         = API_WRITE;
 CALL_API(api_struc);
 return(NO_ACCESS);
 }
if (api_struc->Module.Eye.Value != newvalue)
 {
 api_struc->Module.Eye.Value = oldvalue;
 api_struc->Function         = API_WRITE;
 CALL_API(api_struc);
 return(NO_ACCESS);
 }

/* ------------------ */
/* Write old memory : */
/* ------------------ */
api_struc->Module.Eye.Value = oldvalue;
api_struc->Function         = API_WRITE;
CALL_API(api_struc);
if(api_struc->Status)
 {
 api_struc->Module.Eye.Value = oldvalue;
 api_struc->Function         = API_WRITE;
 CALL_API(api_struc);
 return(NO_ACCESS);
 }

return (STATUS_OK);
}

/****************************************************************************/
RET_ FAR_ CALL_ HL_LMLOGIN(Word ModAd, Word Access, Byte DATAFAR_ * RefKey, Byte DATAFAR_ * VerKey, Word Slot_ID, Byte DATAFAR_ * SearchStr)
/****************************************************************************/
{
Word  result;
Long  OldTimerID;
Word  OldSlot_ID, OldBcnt, OldRemote, OldPort, OldResult;
short i, j;
Byte  SString[80];
Word  sslen = 0;
Word  blocks = 0;

GETSTRUC;
OldRemote  = api_struc->Remote;
OldTimerID = api_struc->Task_ID;
OldPort    = api_struc->Port;
OldSlot_ID = api_struc->Slot_ID;
OldBcnt    = api_struc->Bcnt;

api_struc->ModID            = EYE_DONGLE;
api_struc->Remote           = Access;
api_struc->Slot_ID          = Slot_ID;
api_struc->Module.Eye.ModAd = ModAd;

/* --------------------- */
/* Get TaskID from API : */
/* --------------------- */
#ifndef DOS386
if (api_struc->Task_ID == 0)
  {
  api_struc->Function = API_GET_TASKID;
  CALL_API(api_struc);
  }
#endif

api_struc->Protocol = 0;
api_struc->Port     = 0;
api_struc->Timeout  = 0;
api_struc->PM_Host  = API_XTD_DETECT;

/* -------------------------------------------- */
/* We generated a verify key with TESTAPI.EXE : */
/* -------------------------------------------- */
for (i = 0; i < 8; i++)
  {
  api_struc->ID_Ref[i]    = RefKey[i];
  api_struc->ID_Verify[i] = VerKey[i];
  }

/* ------------------------- */
/* Don't check for HL_SEARCH */
/* ------------------------- */
#ifdef NO_ENVCHK
api_struc->EnvMask |= IGNORE_ENVIRONMENT;
#endif

/* --------------------------------- */
/* Check and prepare search string : */
/* --------------------------------- */
if (SearchStr != 0)
  {
  /* Get length of search string : */
  for (sslen = 0; SearchStr[sslen] != '\0'; sslen++);

  /* Add one for zero : */
  sslen++;

  /* count number of 8 bytes blocks we get : */
  blocks = sslen / 8;
  if ((sslen % 8) != 0)
    blocks++;
  if (blocks > 10)
    return (INVALID_ENV);

  /* Copy search string to stack : */
  for (i = 0; i < sslen; i++)
     SString[i] = SearchStr[i];

  /* Add spaces to fill up last block : */
  for (j = 0; j < (blocks * 8 - sslen); j++)
     SString[i + j] = 0x20;

  /* Now put it into api structure : */
  api_struc->Bcnt = blocks;
  api_struc->Data = SString;
  }

/* --------------------------------------------- */
/* Call the INIT (search for Hardlock) function :*/
/* --------------------------------------------- */
if (Slot_ID == 0)
  {
  api_struc->Function = API_INIT;
  result = CALL_API(api_struc);
  }
 else
  {
  api_struc->Function = API_LMINIT;
  result = CALL_API(api_struc);
  /* Old Low-API returns NOT_INIT error, if function not */
  /* known and API structure hasn't been initialized yet */
  if (result == NOT_INIT)
    return(VERSION_MISMATCH);
  }

if (result == ALREADY_INIT)
  {
  api_struc->Remote  = OldRemote;
  api_struc->Task_ID = OldTimerID;
  api_struc->Port    = OldPort;
  api_struc->Bcnt    = OldBcnt;
  api_struc->Slot_ID = OldSlot_ID;
  return (result);
  }

/* ----------------------------- */
/* No further checks necessary : */
/* ----------------------------- */
if ((result == INVALID_ENV) || (result == CANNOT_OPEN_DRIVER))
  return (result);

/* ----------------------------- */
/* Check, if HL-Server is full : */
/* ----------------------------- */
if (result != STATUS_OK)
  {
  if ((Access & NET_DEVICE) == NET_DEVICE)
    {
    if (Slot_ID == 0)
      {
      /* Check the old way... : */
      /* ---------------------- */
      api_struc->Function = API_LOGIN_INFO;
      if (CALL_API(api_struc) == HLS_FULL)
        return(TOO_MANY_USERS);
      }
     else
      {
      /* Check using LM functions... : */
      /* ----------------------------- */
      OldResult = result;
      api_struc->Function = API_LMPING;
      result = CALL_API(api_struc);

      if (result == HLS_FULL)
        return(TOO_MANY_USERS);

      if ((result == NO_LICENSE) || (OldResult == NO_LICENSE))
        return(NO_LICENSE);

      if (OldResult == INVALID_LIC)
        return(OldResult);

      if (result == NO_DONGLE)
        return(OldResult);

      return (result);
      }
    }
  return (result);
  }

/* ------------------------------------------ */
/* Login to HL-Server if Hardlock is remote : */
/* ------------------------------------------ */
if (api_struc->Remote == NET_DEVICE)
  {
  api_struc->Function = API_LOGIN;
  result              = CALL_API(api_struc);
  if(result)
    {
    api_struc->Function = API_DOWN;
    CALL_API(api_struc);
    }
  }

return (result);
}

/****************************************************************************/
/****************************************************************************/
/* The following functions map the old Hardlock Calls on the new API. These */
/* functions are defined only for compatibility reasons.                    */
/* !!! Don't mix old and new functions. Don't use if it is not necessary.!!!*/
/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
void FAR_ CALL_ INT_ON (void)
/****************************************************************************/
{return;}

/****************************************************************************/
void FAR_ CALL_ INT_OFF (void)
/****************************************************************************/
{return;}

/****************************************************************************/
void FAR_ CALL_ HL_ON (Word Port, Word ModAd)
/****************************************************************************/
{
GETSTRUC;
api_struc->ModID            = EYE_DONGLE;
api_struc->Remote           = LOCAL_DEVICE;
api_struc->Module.Eye.ModAd = ModAd;
api_struc->Port             = Port;
api_struc->Function         = API_INIT;
CALL_API(api_struc);
if(api_struc->Status) return;

api_struc->Function = API_LOGIN;
CALL_API(api_struc);
if(api_struc->Status)
 {
 api_struc->Function = API_DOWN;
 CALL_API(api_struc);
 }

return;
}

/****************************************************************************/
void FAR_ CALL_ HL_OFF (Word Port)
/****************************************************************************/
{
GETSTRUC;
api_struc->Port     = Port;
api_struc->Function = API_LOGOUT;
CALL_API(api_struc);

api_struc->Function = API_DOWN;
CALL_API(api_struc);
if (api_struc->Status)
  {
  api_struc->Function = API_FORCE_DOWN;
  CALL_API(api_struc);
  }
return;
}

/****************************************************************************/
Word FAR_ CALL_ K_EYE (Word Port, char  DATAFAR_ *Inp, Word BlkCnt)
/****************************************************************************/
{
GETSTRUC;
api_struc->Port     = Port;
api_struc->Data     = Inp;
api_struc->Bcnt     = BlkCnt;
api_struc->Function = API_KEYE;
CALL_API(api_struc);
api_struc->Bcnt     = 0;

if (api_struc->Status)
  return (0);
return (1);
}

/****************************************************************************/
void FAR_ CALL_ HL_WR (Word Port, Word Reg, Word Val)
/****************************************************************************/
{
GETSTRUC;
if(Reg >= 48 && Reg <= 63)
  {
  api_struc->Port             = Port;
  api_struc->Module.Eye.Reg   = Reg;
  api_struc->Module.Eye.Value = Val;
  api_struc->Function         = API_WRITE;
  CALL_API(api_struc);
  }
return;
}

/****************************************************************************/
Word FAR_ CALL_ HL_RD (Word Port, Word Reg)
/****************************************************************************/
{
GETSTRUC;
if(Reg <= 63)
  {
  api_struc->Port           = Port;
  api_struc->Module.Eye.Reg = Reg;
  api_struc->Function       = API_READ;
  CALL_API(api_struc);
  if (api_struc->Status == STATUS_OK)
    return (api_struc->Module.Eye.Value);
  }

return (0);
}

/****************************************************************************/
RET_ FAR_ CALL_ HL_CALC (Word i1, Word i2, Word i3, Word i4)
/****************************************************************************/
{
unsigned short Shift1, Shift2;

GETSTRUC;
api_struc->Function = 16;   /* Function: query Hardlock in compatible mode */

Shift1 = i1 | (i2 << 8);
Shift2 = i3 | (i4 << 8);
api_struc->Timeout = (Long) Shift1 | (Long) (Shift2 * 0x10000L);

CALL_API(api_struc);
if(api_struc->Status)
  return 0;
 else
  return api_struc->ShortLife;
}

