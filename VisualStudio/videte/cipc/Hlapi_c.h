#include "fastapi.h"

/* --------------------- */
/* Function prototypes : */
/* --------------------- */
#ifdef __cplusplus
  extern "C" {
#endif

RET_ FAR_ CALL_ HL_LOGIN     (Word ModAd, Word Access, Byte DATAFAR_ *RefKey, Byte DATAFAR_ *VerKey);
RET_ FAR_ CALL_ HL_LOGOUT    (void);
RET_ FAR_ CALL_ HL_AVAIL     (void);
RET_ FAR_ CALL_ HL_PORTINF   (void);
RET_ FAR_ CALL_ HL_ACCINF    (void);
RET_ FAR_ CALL_ HL_USERINF   (void);
RET_ FAR_ CALL_ HL_MAXUSER   (void);
RET_ FAR_ CALL_ HL_MEMINF    (void);
RET_ FAR_ CALL_ HL_CODE      (void DATAFAR_ *Data, Word Count);
RET_ FAR_ CALL_ HL_WRITE     (Word Reg, Word Value);
RET_ FAR_ CALL_ HL_READ      (Word Reg, Word DATAFAR_ *Value);
RET_ FAR_ CALL_ HL_READBL    (Byte DATAFAR_ *Eeprom);
RET_ FAR_ CALL_ HL_WRITEBL   (Byte DATAFAR_ *Eeprom);
RET_ FAR_ CALL_ HL_ABORT     (void);
RET_ FAR_ CALL_ HL_VERSION   (void);
RET_ FAR_ CALL_ HL_HLSVERS   (void);
RET_ FAR_ CALL_ HL_SELECT    (HL_API DATAFAR_ *hl_ptr);

RET_ FAR_ CALL_ HL_LMLOGIN   (Word ModAd, Word Access, Byte DATAFAR_ *RefKey, Byte DATAFAR_ *VerKey, Word SlotID, Byte DATAFAR_ *SearchStr);

#ifndef __OS2__
/****************************************************************************/
/* The following functions map the old Hardlock Calls on the new API. These */
/* functions are defined only for compatibility reasons.                    */
/* !!! Don't mix old and new functions. Don't use if it is not necessary.!!!*/
/****************************************************************************/
void FAR_ CALL_ HL_ON        (Word Port, Word ModAd);
void FAR_ CALL_ HL_OFF       (Word Port);
Word FAR_ CALL_ K_EYE        (Word Port, char DATAFAR_ *Inp, Word BlkCnt);
void FAR_ CALL_ HL_WR        (Word Port, Word Reg, Word Val);
Word FAR_ CALL_ HL_RD        (Word Port, Word Reg);
void FAR_ CALL_ INT_ON       (void);
void FAR_ CALL_ INT_OFF      (void);
#endif
RET_ FAR_ CALL_ HL_CALC      (Word i1, Word i2, Word i3, Word i4);

#ifdef __cplusplus
  };
#endif
/* eof */
