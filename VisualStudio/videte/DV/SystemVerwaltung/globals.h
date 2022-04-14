/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: globals.h $
// ARCHIVE:		$Archive: /DV/SystemVerwaltung/globals.h $
// CHECKIN:		$Date: 22.02.01 11:06 $
// VERSION:		$Revision: 1 $
// LAST CHANGE:	$Modtime: 21.09.00 14:37 $
// BY AUTHOR:	$Author: Uwe $
// $Nokeywords:$

#ifndef __SEC_GLOBALS_H
#define __SEC_GLOBALS_H

/////////////////////////////////////////////////////////////////////////////////////
// Defines for Images for Tree-Controls
// Note:Order must be the same as in images.bmp
#define IMAGE_NULL			0
#define IMAGE_INPUT			1
#define IMAGE_LOCK			2
#define IMAGE_CAMERA		3
#define IMAGE_COCO			4
#define IMAGE_CAMERA_FBAS	5
#define IMAGE_CAMERA_SVHS	6
#define IMAGE_SAVE			7
#define IMAGE_RELAY			8
#define IMAGE_AKU			9
#define IMAGE_CALL			10
#define IMAGE_MICO			11
#define IMAGE_TIMER			12
#define IMAGE_TIMER_LOCK	13
#define IMAGE_ISDN			14
#define IMAGE_BLUE			15
#define IMAGE_HOST			16
#define IMAGE_PERM			17
#define IMAGE_USER			18
#define IMAGE_PROCESS		19
#define IMAGE_ACTIVATION	20
#define IMAGE_HARDWARE		21
#define IMAGE_SOFTWARE		22
#define IMAGE_SYSTEM		23
#define IMAGE_SOCKET		24
#define IMAGE_SIM			25
#define IMAGE_GAA			26
#define IMAGE_STM			27
#define IMAGE_MESZ			28
#define IMAGE_SDI			29
#define IMAGE_RS232			30
#define IMAGE_AUTOLOGOUT	31
#define IMAGE_PTUNIT		32
#define IMAGE_LINK			33
#define IMAGE_FIELDMAP		34
#define IMAGE_NOTIFICATION	35
#define IMAGE_DATABASE		36
#define IMAGE_DRIVES		37
#define IMAGE_ARCHIVS		38
#define IMAGE_SMS			39
#define IMAGE_SECURITY		40
#define IMAGE_BACKUP		41
#define IMAGE_ACTUAL_IMAGE	42
#define IMAGE_IMAGE_CALL	43
#define IMAGE_CHECK_CALL	44
#define IMAGE_JACOB			45
#define IMAGE_GEMOS			46
#define IMAGE_EMAIL			47
#define IMAGE_TOBS			48

#define IMAGE_NR_IMAGES		49

/////////////////////////////////////////////////////////////////////////////
#define PRIOR_MAX			1
#define PRIOR_MIN			5
#define PRIOR_STEPS			5

/////////////////////////////////////////////////////////////////////////////
#define MICO1				1
#define MICO2				2
#define MICO3				3
#define MICO4				4

/////////////////////////////////////////////////////////////////////////////
#define JACOB1				1
#define JACOB2				2
#define JACOB3				3
#define JACOB4				4

/////////////////////////////////////////////////////////////////////////////
#define ISDN1				1
#define ISDN2				2

/////////////////////////////////////////////////////////////////////////////
#define PT1					1
#define PT2					2

/////////////////////////////////////////////////////////////////////////////
#define NR_INPUTS_MICO_ISA				8
#define NR_OUTPUTS_CAMERA_MICO_ISA		8
#define NR_OUTPUTS_RELAY_MICO_ISA		4

#define NR_INPUTS_MICO_PCI				8
#define NR_OUTPUTS_CAMERA_MICO_PCI		16
#define NR_OUTPUTS_RELAY_MICO_PCI		4

#define NR_INPUTS_JACOB					16
#define NR_OUTPUTS_CAMERA_JACOB			16
#define NR_OUTPUTS_RELAY_JACOB			4

#define NR_INPUTS_COVI					8
#define NR_OUTPUTS_CAMERA_COVI			8
#define NR_OUTPUTS_RELAY_COVI			4

#define NR_INPUTS_COCO					8
#define NR_OUTPUTS_CAMERA_COCO			8
#define NR_OUTPUTS_RELAY_COCO			4
#define NR_INPUTS_GAA					8
#define NR_INPUTS_AKU					8
#define NR_OUTPUTS_AKU					8
#endif
