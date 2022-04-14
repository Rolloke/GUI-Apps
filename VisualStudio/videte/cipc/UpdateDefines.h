/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: UpdateDefines.h $
// ARCHIVE:		$Archive: /Project/CIPC/UpdateDefines.h $
// CHECKIN:		$Date: 3/16/06 8:57a $
// VERSION:		$Revision: 14 $
// LAST CHANGE:	$Modtime: 3/16/06 8:52a $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#ifndef __UPDATE_DEFINES_H
#define __UPDATE_DEFINES_H

// Destination: 0=AppDir, 1=Windows, 2=Windows/System, 3=Root
//				4=use sFileName
//				5=new String Interface in sFileName
#define RFU_APPDIR			 0x00
#define RFU_WINDOWS			 0x01
#define RFU_SYSTEM			 0x02
#define RFU_ROOT			 0x03
#define RFU_FULLPATH		 0x04
#define RFU_STRING_INTERFACE 0x05

#define RFU_DELETE			 0x08
#define RFU_EXECUTE			 0x10
#define RFU_DIVIDE_IN_BITS	 0x20

#define CFU_FILE				 0x01
#define CFU_DIRECTORY			 0x05
#define CFU_GET_LOGICAL_DRIVES   0x06
#define CFU_GET_DISK_FREE_SPACE	 0x07
#define CFU_GET_OS_VERSION		 0x08
#define CFU_GET_SYSTEM_INFO		 0x09
#define CFU_GET_CURRENT_TIME	 0x0A
#define CFU_GET_SOFT_VERSION	 0x0B
#define CFU_GET_FILE_VERSION	 0x0C
#define CFU_GLOBAL_MEMORY_STATUS 0x0D
#define CFU_EXPORT_REGISTRY		 0x0E
#define CFU_DELETE_FILE			 0x0F
#define CFU_EXECUTE_FILE		 0x10
#define CFU_WWW_FILE			 0x11
#define CFU_TIME_ZONE_INFO		 0x12
#define CFU_PODUCT_INFO			 0x13
#define CFU_ENUMREGKEYS			 0x14
#define CFU_ENUMREGVALUES		 0x15
#define CFU_GETREGKEY			 0x16
#define CFU_SETREGKEY			 0x17
#define CFU_DELETEREGKEY		 0x18

#endif
