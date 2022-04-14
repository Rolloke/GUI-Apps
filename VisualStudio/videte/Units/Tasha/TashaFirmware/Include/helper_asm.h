/////////////////////////////////////////////////////////////////////////////
// PROJECT:		JaCobDll
// FILE:		$Workfile: helper_asm.h $
// ARCHIVE:		$Archive: /Project/Units/Tasha/TashaFirmware/Include/helper_asm.h $
// CHECKIN:		$Date: 9.01.04 13:01 $
// VERSION:		$Revision: 2 $
// LAST CHANGE:	$Modtime: 9.01.04 13:01 $
// BY AUTHOR:	$Author: Christian.lohmeier $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#ifndef __HELPER_ASM_H__
#define __HELPER_ASM_H__

// delete macros for saving Regs

////////////////////////////////////////////////////////////////
 

#define SAVE_ASM_REGS\
		[--SP] = (P5:3);\
		[--SP] = (R7:4)

#define RESTORE_ASM_REGS\
		(R7:4) =	[SP++];\
		(P5:3) =	[SP++]
		 

#endif //__HELPER_ASM_H__

