#ifndef __CSORTFUNCTIONS_H_INCLUDED
#define __CSORTFUNCTIONS_H_INCLUDED

#define STRICT
#include <WINDOWS.H>
#include <float.h>

class CSortFunctions
{
public:
   static void SetSortDirection(bool bAsc)
   {
      if (bAsc) gm_nDirection = 1;
      else      gm_nDirection = -1;
      gm_bChanged = gm_bEqual = false;
   }
   static int __cdecl SortFloat(const void*, const void*);
   static int __cdecl SortDouble(const void*, const void*);
	static int __cdecl SortChassisType(const void *, const void *);

   static bool gm_bChanged;
   static bool gm_bEqual;
private:
   static int  gm_nDirection;
};

#endif //__CSORTFUNCTIONS_H_INCLUDED
