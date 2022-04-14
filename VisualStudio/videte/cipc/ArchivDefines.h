/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: ArchivDefines.h $
// ARCHIVE:		$Archive: /Project/CIPC/ArchivDefines.h $
// CHECKIN:		$Date: 20.01.05 15:23 $
// VERSION:		$Revision: 15 $
// LAST CHANGE:	$Modtime: 20.01.05 15:23 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$
//
#ifndef __INC_ARCHIV_DEFINES_H__
#define __INC_ARCHIV_DEFINES_H__

#define CAR_IS_RING			((BYTE)0x01)	 // standard ring archiv
#define CAR_IS_ALARM		((BYTE)0x02)	 // alarm archiv	
#define CAR_IS_SAFE_RING	((BYTE)0x04)	 // sicherungsring, vorring
#define CAR_IS_SEARCH		((BYTE)0x08)	 // never used
#define CAR_IS_BACKUP		((BYTE)0x10)	 // backup archiv read/search only !
#define CAR_CAN_DELETE		((BYTE)0x20)	 // archiv sequence read/search only by permission!
#define CAR_IS_SUSPECT		((BYTE)0x40)	 // archiv sequence suspect!
#define CAR_IS_ALARM_LIST	((BYTE)0x80)	 // alarm list archiv

#define	SEARCH_SUCCESS					(1)
#define	SEARCH_FAILED_NO_OPEN_FILES		(2)
#define	SEARCH_CANCELED					(3)

#define	START_SIZE_MB					(150)
#define	LOCAL_SEARCH_RESULT_ARCHIV_NR	(255)

enum DriveType
{	
	INVALID_DRIVE	=	0,
	LOCAL_DRIVE		=	1,
	REMOTE_DRIVE	=	2,
	CDROM_DRIVE		=	3
};

enum ArchivType
{
	NO_ARCHIV = 0,
	ALARM_ARCHIV = 1,
	RING_ARCHIV = 2,
	SICHERUNGS_RING_ARCHIV = 3,
	SUCHERGEBNIS_ARCHIV = 4,
	BACKUP_ARCHIV = 5,
	VERDACHT_ARCHIV = 6,
	ALARM_LIST_ARCHIV = 7
};

enum NavigationType
{
	NAVIGATION_ALL       = 0,
	NAVIGATION_TRACK     = 1,
	NAVIGATION_SUSPECT   = 2,
	NAVIGATION_FULL_HOUR = 3,
	NAVIGATION_LAST	     = 4
};

// Strings dürfen höchstens 32 Byte lang sein !!!
#define	STRING_NO_ARCHIV					_T("NO_ARCHIV")
#define	STRING_ALARM_ARCHIV				_T("ALARM_ARCHIV")
#define	STRING_RING_ARCHIV				_T("RING_ARCHIV")
#define	STRING_SICHERUNGS_RING_ARCHIV	_T("SICHERUNGS_RING_ARCHIV")
#define	STRING_SUCHERGEBNIS_ARCHIV		_T("SUCHERGEBNIS_ARCHIV")

#define	MIN_ARCHIV_SIZE_MB		15

typedef __int64 DriveSize;
typedef __int64 ArchivSize;

inline const _TCHAR* GetArchivTypeString(ArchivType at)
{
	switch (at)
	{
	case NO_ARCHIV:
		return STRING_NO_ARCHIV;
	case ALARM_ARCHIV:
		return STRING_ALARM_ARCHIV;
	case RING_ARCHIV:
		return STRING_RING_ARCHIV;
	case SICHERUNGS_RING_ARCHIV:
		return STRING_SICHERUNGS_RING_ARCHIV;
	case SUCHERGEBNIS_ARCHIV:
		return STRING_SUCHERGEBNIS_ARCHIV;
	}
	return NULL;
}

inline ArchivType GetArchivType(LPCTSTR p)
{
	if (_tcscmp(p, STRING_ALARM_ARCHIV)==0){
		return ALARM_ARCHIV;
	}
	else if (_tcscmp(p, STRING_RING_ARCHIV)==0){
		return RING_ARCHIV;
	}
	else if (_tcscmp(p, STRING_SICHERUNGS_RING_ARCHIV)==0){
		return SICHERUNGS_RING_ARCHIV;
	}
	else if (_tcscmp(p, STRING_SUCHERGEBNIS_ARCHIV)==0){
		return SUCHERGEBNIS_ARCHIV;
	}
	return NO_ARCHIV;
}
//////////////////////////////////////////////////////////////////////
#define DFN_ARCHIV_NR	_T("ARC_NR")
#define DFN_KAMERA_NR	_T("SKAMERA")
#define DFN_REC_DATE		_T("DATUM")
#define DFN_REC_TIME		_T("ZEIT")
#define DFN_PICTURETYP	_T("TYP")
#define DFN_COMPRESSION	_T("COMP")
#define DFN_FULL			_T("VOLLBILD")
#define DFN_DTP_TIME		_T("DTP_ZEIT")
#define DFN_DTP_DATE		_T("DTP_DATE")
#define DFN_DTP_CAMERA	_T("GKAMERA")
#define DFN_GA_NR			_T("GA_NR")
#define DFN_TA_NR			_T("TA_NR")
#define DFN_KTO_NR		_T("KTO_NR")
#define DFN_CARD_TYP		_T("CARD_TYP")
#define DFN_GAA_TYP		_T("GAA_TYP")
#define DFN_BLZ			_T("BLZ")
#define DFN_VALUE			_T("BETRAG")
#define DFN_CURRENCY		_T("WAEHRUNG")
#define DFN_STATION		_T("STATION")
#define DFN_CAM_NAME		_T("KAM_NAME")
#define DFN_VAR1			_T("VAR1")
#define DFN_VAR2			_T("VAR2")
#define DFN_VAR3			_T("VAR3")
#define DFN_COMMENT		_T("COMMENT")
#define DFN_PICT_NR		_T("PICT_NR")
//////////////////////////////////////////////////////////////////////

#endif
