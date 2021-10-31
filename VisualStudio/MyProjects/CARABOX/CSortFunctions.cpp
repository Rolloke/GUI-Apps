#include "CSortFunctions.h"
#include "BoxStructures.h"

int  CSortFunctions::gm_nDirection = 1;
bool CSortFunctions::gm_bChanged   = false;
bool CSortFunctions::gm_bEqual     = false;

int __cdecl CSortFunctions::SortDouble(const void* p1, const void* p2)
{
   int nRet = 0;
   if      (*(double*)p1 > *(double*)p2) nRet =  gm_nDirection;
   else if (*(double*)p1 < *(double*)p2) nRet = -gm_nDirection;
   if      (nRet <  0) gm_bChanged = true;
   else if (nRet == 0) gm_bEqual   = true;
   return nRet;
}

/******************************************************************************
* Definition :
* Zweck      :
* Aufruf     :
* Parameter (EA):
* Funktionswert :
******************************************************************************/
int __cdecl CSortFunctions::SortFloat(const void* p1, const void* p2)
{
   int nRet = 0;
   if      (*(float*)p1 < MIN_ACC_FLOAT)
   {
      *((float*)p1) = 0.0;
      nRet          =  1;
   }
   else if (*(float*)p2 < MIN_ACC_FLOAT)
   {
      *((float*)p2) = 0.0;
      nRet          = -1;
   }
   else if (*((float*)p1) > *((float*)p2)) nRet =  gm_nDirection;
   else if (*((float*)p1) < *((float*)p2)) nRet = -gm_nDirection;
   if      (nRet <  0) gm_bChanged = true;
   else if (nRet == 0) gm_bEqual   = true;
   return nRet;
}

/******************************************************************************
* Definition :
* Zweck      :
* Aufruf     :
* Parameter (EA):
* Funktionswert :
******************************************************************************/
int __cdecl CSortFunctions::SortChassisType(const void *p1, const void *p2)
{
   int nRet = 0;
   ChassisData *pCD1 = (ChassisData*) p1;
   ChassisData *pCD2 = (ChassisData*) p2;
   if      (pCD1->IsHidden() || pCD1->IsDummy())   nRet = -1;
   else if (pCD2->IsHidden() || pCD2->IsDummy())   nRet =  1;
   else if (pCD1->nChassisTyp > pCD2->nChassisTyp) nRet =  gm_nDirection;
   else if (pCD1->nChassisTyp < pCD2->nChassisTyp) nRet = -gm_nDirection;

   if      (nRet <  0) gm_bChanged = true;
   else if (nRet == 0) gm_bEqual   = true;
   return nRet;
}

