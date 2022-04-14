/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: globals.h $
// ARCHIVE:		$Archive: /Project/SystemVerwaltung/globals.h $
// CHECKIN:		$Date: 8.06.06 21:26 $
// VERSION:		$Revision: 51 $
// LAST CHANGE:	$Modtime: 8.06.06 21:25 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#ifndef __SEC_GLOBALS_H
#define __SEC_GLOBALS_H

#include "wkclasses\wk_string.h"

/////////////////////////////////////////////////////////////////////////////
static TCHAR BASED_CODE szFieldMapSection[]	= _T("FieldMap");
static TCHAR BASED_CODE szDontUse[]	= _T("dontuse");

/////////////////////////////////////////////////////////////////////////////////////
// Defines for Images for Tree-Controls
// Note:Order must be the same as in images.bmp
#define IMAGE_NULL			0
#define IMAGE_INPUT			1
#define IMAGE_LOCK			2
#define IMAGE_CAMERA		3
#define IMAGE_Q_UNIT		4
#define IMAGE_CAMERA_FBAS	5
#define IMAGE_CAMERA_SVHS	6
#define IMAGE_SAVE			7
#define IMAGE_RELAY			8
#define IMAGE_IP_CAMERA		9
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
#define IMAGE_GUARD_TOUR	49
#define IMAGE_SAVIC			50
#define IMAGE_ICPCON		51
#define IMAGE_MULTI_MONITOR	52
#define IMAGE_YUTA			53
#define IMAGE_VIDEO_OUT		54
#define IMAGE_AUDIO  		55
#define IMAGE_USB_CAM  		56
#define IMAGE_AUDIO_IN 		57
#define IMAGE_AUDIO_OUT		58
#define IMAGE_TASHA			59
#define IMAGE_LANGUAGE		60
#define IMAGE_JOYSTICK		61
#define IMAGE_QUAD			62
#define IMAGE_MAINTENANCE	63
#define IMAGE_PTZ_PRESET	64
#define IMAGE_ALARM_LIST	65
#define IMAGE_UPLOAD		66
#define IMAGE_MISCELLANEOUS	67
#define IMAGE_NR_IMAGES		68

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
#define SAVIC1				1
#define SAVIC2				2
#define SAVIC3				3
#define SAVIC4				4

/////////////////////////////////////////////////////////////////////////////
#define TASHA1				1
#define TASHA2				2
#define TASHA3				3
#define TASHA4				4

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

#define NR_INPUTS_JACOB						16
#define NR_OUTPUTS_CAMERA_JACOB				16
#define NR_OUTPUTS_RELAY_JACOB				4
#define NR_OUTPUTS_CAMERA_JACOB_RESTRICTED	8

#define NR_INPUTS_SAVIC						4
#define NR_OUTPUTS_CAMERA_SAVIC				4
#define NR_OUTPUTS_RELAY_SAVIC				1

#define NR_INPUTS_TASHA						9
#define NR_OUTPUTS_CAMERA_TASHA				8
#define NR_OUTPUTS_RELAY_TASHA				1

#define NR_INPUTS_Q						16
#define NR_OUTPUTS_CAMERA_Q				32
#define NR_OUTPUTS_RELAY_Q				4

#define NR_INPUTS_COVI					8
#define NR_OUTPUTS_CAMERA_COVI			8
#define NR_OUTPUTS_RELAY_COVI			4

#define NR_INPUTS_COCO					8
#define NR_OUTPUTS_CAMERA_COCO			8
#define NR_OUTPUTS_RELAY_COCO			4
#define NR_INPUTS_GAA					8
#define NR_INPUTS_AKU					8
#define NR_OUTPUTS_AKU					8

#define   INPUT_OFF    0
#define   INPUT_MELDER 1
#define   INPUT_BLOCK  2

#define   CAMERA_OFF  0
#define   CAMERA_FBAS 1
#define   CAMERA_SVHS 2


#define HTML_TEXT_TOP    HTML_BEGIN_TAG HTML_LINEFEED \
                         HTML_HEAD_TAG HTML_LINEFEED \
						 HTML_CODEPAGE_METATAG HTML_LINEFEED \
                         HTML_GENERATOR_METATAG HTML_LINEFEED \
                         HTML_MAPTYPE_METATAG HTML_LINEFEED \
                         HTML_TITLE_TAG HTML_LINEFEED \
                         HTML_HEADEND_TAG HTML_LINEFEED 

#define HTML_TEXT_BOTTOM HTML_BODYEND_TAG HTML_LINEFEED \
                         HTML_END_TAG HTML_LINEFEED

#endif
