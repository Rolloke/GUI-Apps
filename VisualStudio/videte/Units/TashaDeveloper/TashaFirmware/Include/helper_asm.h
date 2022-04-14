/////////////////////////////////////////////////////////////////////////////
// PROJECT:		JaCobDll
// FILE:		$Workfile: helper_asm.h $
// ARCHIVE:		$Archive: /Project/Units/Tasha(Developer)/TashaFirmware/Include/helper_asm.h $
// CHECKIN:		$Date: 5.01.04 9:54 $
// VERSION:		$Revision: 1 $
// LAST CHANGE:	$Modtime: 5.01.04 9:25 $
// BY AUTHOR:	$Author: Martin.lueck $
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
		 

#endif __HELPER_ASM_H__

