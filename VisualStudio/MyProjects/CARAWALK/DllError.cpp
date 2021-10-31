#include "stdafx.h"
#include "DllError.h"

void SetDllError(int nError)
{
   SetLastError(nError);
}

