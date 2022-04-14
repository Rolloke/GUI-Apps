
// stdafx.cpp: Quelldatei, die nur die Standard-Includes einbindet
// SchrittmotorLPT.pch ist der vorkompilierte Header
// stdafx.obj enthält die vorkompilierten Typinformationen

#include "stdafx.h"


BOOL	_stdcall IsInpOutDriverOpen()  //Returns TRUE if the InpOut driver was opened successfully
{
    return TRUE;
}
UCHAR   _stdcall DlPortReadPortUchar (USHORT port)
{
    return 0;
}
void    _stdcall DlPortWritePortUchar(USHORT port, UCHAR Value)
{

}
