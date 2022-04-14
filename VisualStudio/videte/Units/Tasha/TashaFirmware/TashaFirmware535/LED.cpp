/////////////////////////////////////////////////////////////////////////////
// PROJECT:		TashaFirmware535
// FILE:		$Workfile: LED.cpp $
// ARCHIVE:		$Archive: /Project/Units/Tasha/TashaFirmware/TashaFirmware535/LED.cpp $
// CHECKIN:		$Date: 19.03.04 13:01 $
// VERSION:		$Revision: 4 $
// LAST CHANGE:	$Modtime: 15.03.04 15:32 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include <cdefblackfin.h>
#include <CdefBF535.h>
#include "LED.h"

//////////////////////////////////////////////////////////////////
void SetLED1()
{
	*pFIO_DIR 	 = *pFIO_DIR | PF_LED1;			// PF15 auf Ausgang
	*pFIO_FLAG_C = PF_LED1;						// PF15 auf '0'
}

//////////////////////////////////////////////////////////////////
void ClearLED1()
{
	*pFIO_DIR 	 = *pFIO_DIR | PF_LED1;			// PF15 auf Ausgang
	*pFIO_FLAG_S = PF_LED1;						// PF15 auf '1'
}

//////////////////////////////////////////////////////////////////
void SetLED2()
{
	*pFIO_DIR 	 = *pFIO_DIR | PF_LED2;			// PF14 auf Ausgang
	*pFIO_FLAG_C = PF_LED2;						// PF14 auf '0'
}

//////////////////////////////////////////////////////////////////
void ClearLED2()
{
	*pFIO_DIR 	 = *pFIO_DIR | PF_LED2;			// PF14 auf Ausgang
	*pFIO_FLAG_S = PF_LED2;						// PF14 auf '1'
}
