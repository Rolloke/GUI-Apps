/////////////////////////////////////////////////////////////////////////////
// PROJECT:		AkuUnit
// FILE:		$Workfile: AkuReg.h $
// ARCHIVE:		$Archive: /Project/Units/Aku/AkuUnit/AkuReg.h $
// CHECKIN:		$Date: 5.08.98 9:52 $
// VERSION:		$Revision: 3 $
// LAST CHANGE:	$Modtime: 5.08.98 9:52 $
// BY AUTHOR:	$Author: Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#ifndef __AKUREG_H__
#define __AKUREG_H__

#define ALARM_STATUS_OFFSET			0x00
#define ALARM_EDGE_SEL_OFFSET		0x01
#define ALARM_ACK_OFFSET			0x02
#define ALARM_CUR_STATUS_OFFSET		0x03
#define RELAY_OFFSET				0x04
#define EXT_RELAY_OFFSET			0x05bk

// Bitdefininitionen des Relay-Registers
#define RELAY0_BIT				0x00
#define RELAY1_BIT				0x01
#define RELAY2_BIT				0x02
#define RELAY3_BIT				0x03
#define RELAY4_BIT				0x04
#define RELAY5_BIT				0x05
#define RELAY6_BIT				0x06
#define RELAY7_BIT				0x07

#endif  // __AKUREG_H__
