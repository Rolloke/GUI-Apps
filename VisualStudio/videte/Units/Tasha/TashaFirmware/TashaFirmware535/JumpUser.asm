/////////////////////////////////////////////////////////////////////////////
// PROJECT:		TashaFirmware535
// FILE:		$Workfile: TashaFirmware535.cpp $
// ARCHIVE:		$Archive: /Project/Units/Tasha/TashaFirmware/TashaFirmware535/TashaFirmware535.cpp $
// CHECKIN:		$Date: 19.01.04 11:18 $
// VERSION:		$Revision: 82 $
// LAST CHANGE:	$Modtime: 19.01.04 11:18 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include <defBF535.h>
#include "m21535.h"
#include "basicmon.h"

#if defined(_HAWK_35_) // [ _HAWK_35_ 
//------------------------------------------------------------------------------
	.extern start;
//	.extern _SetVCOMultiplier;
		
	.section jump_user_program;
	.align 2;
	jump start;
//	jump _SetVCOMultiplier;
#else              // ] [ !_HAWK_35_ 
	#error "Board not defined"
#endif             // ] _HAWK_35_ 