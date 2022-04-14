/////////////////////////////////////////////////////////////////////////////
// PROJECT:		JaCobDll
// FILE:		$Workfile: tm.h $
// ARCHIVE:		$Archive: /Project/Units/Tasha/TashaFirmware/tm.h $
// CHECKIN:		$Date: 14.02.02 9:49 $
// VERSION:		$Revision: 11 $
// LAST CHANGE:	$Modtime: 14.02.02 9:48 $
// BY AUTHOR:	$Author: Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#ifndef __TM_H__
#define __TM_H__

#if defined(__cplusplus)
extern "C" {
#endif

//#define TM_DEBUG

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <assert.h>
#include <tm1/mmio.h>
#include <ops/custom_defs.h>
#include <tm1/tmVI.h>
#include <tm1/tmVO.h>
#include <tm1/tmICP.h>
#include <tm1/tmAvFormats.h>
#include <tm1/tmVImmio.h>
#include <tm1/tmVOmmio.h>
#include <tm1/tmInterrupts.h>
#include <tm1/tmProcessor.h>
#include <tmlib/tmlibc.h>
#include <tmlib/dprintf.h>
#include <tmlib/AppModel.h>
#include <tmlib/tmtypes.h>
#include <tmlib/AppSem.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>
#include <common/va_tcs.h>

#include "sys_conf.h"
#include <D:/PROGRAMME/DEVELOPER/TRIMEDIA/SDE/apps/include/tmLibappErr.h>
#include <D:/PROGRAMME/DEVELOPER/TRIMEDIA/SDE/apps/include/tmalVtransICP.h>
#include <D:/PROGRAMME/DEVELOPER/TRIMEDIA/SDE/apps/include/tmos.h>
#include "D:/PROGRAMME/DEVELOPER/TRIMEDIA/SDE/MJPGLib/include/mjpg_enc.h"

#include <psos.h>
#include <probe.h>

//#include <unistd.h>

#define  NUM_BUF_ENTRIES        4

#define  VID_RDY_VI             1    /* buffer is ready for vi to use */
#define  VID_RDY_MM             2    /* buffer is ready for mm to use */
#define  VID_RDY_VO             4    /* buffer is ready for vo to use */

#define  CLOCK_SPEED			(178750000)

#include "helper.h"
#include "CInt64.h"
#include "..\\TashaUnit\TMMEssages.h"

typedef struct _exvideoBuf
{   
	VIDEOBUFFER	videoBuffer;
	UInt32      flag;				// flags used in buffer management
	UInt16		wSource;			// Kameranummer
	UInt32		dwProzessID;		// ProzessID
	DataType	eReqCompressType;	// Gewünschte Kompression
} exVideoBuffer, *pexVideoBufffer;

#if defined(__cplusplus)
}
#endif
	
#endif __TM_H__
