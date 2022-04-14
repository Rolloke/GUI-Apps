/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: inifile.h $
// ARCHIVE:		$Archive: /Project/CIPC/inifile.h $
// CHECKIN:		$Date: 15.07.98 10:21 $
// VERSION:		$Revision: 5 $
// LAST CHANGE:	$Modtime: 15.07.98 10:20 $
// BY AUTHOR:	$Author: Hedu $
// $Nokeywords:$

#ifndef __INIFILE_H_
#define __INIFILE_H_

#include "WK_Profile.h"

	// Allgemeine ENT's
	#define ENT_GROUP				1
	#define ENT_XPOS				10000
	#define ENT_YPOS				10001    
	#define ENT_GLOBAL				10002
	#define ENT_LEFT				10003
	#define ENT_TOP					10004
	#define ENT_WIDTH				10005
	#define ENT_HEIGHT				10006


// Section "Output"
#define SEC_NAME_OUTPUT			"Output"	
#define SEC_OUTPUT				2	                                                                        
	#define ENT_OUTPNR				1

// Section "Capi"
#define SEC_NAME_CAPI			"Capi"
#define SEC_CAPI				3
	#define ENT_NRXBUF				0
	#define ENT_NRBCHANNELS			1
	#define ENT_PROTOCOL	 		2
	#define ENT_HANDSAKELEVEL		3

// Section "timer"
#define SEC_TIMER				4
	#define ENT_TNR					1
  	#define ENT_MAXTIMER			1000

// Section "prozess"
#define SEC_PROZESS				5
	#define ENT_PNR					1
	#define ENT_MAXPROZESS			1000

// Section "user"
#define SEC_USER				6
	#define ENT_USERNR				1
	#define ENT_MAXUSER				1000

// Section "input- und outputgroups"
#define SEC_INPUTGROUP			7
#define SEC_OUTPUTGROUP			8
	#define ENT_MAXGROUPS			1000

#define SEC_NAME_CONTROLLER		"ISDN-Controller"	
#define SEC_CONTROLLER			9
	#define ENT_NR					1

#define SEC_NAME_INP_LIST		"ListPositions\\InputList"
#define SEC_INP_LIST			10
#define SEC_NAME_OUTP_LIST		"ListPositions\\OutputList"
#define SEC_OUTP_LIST			11

#define SEC_NAME_OWNPICT		"PictureGroups"
#define SEC_OWNPICT				12
	#define ENT_SHOW_INPUTLIST		1000
	#define ENT_SHOW_OUTPUTLIST		1002
	#define ENT_MAXIMIZE			1003
	#define ENT_GROUPS				1004
	#define ENT_OVERLAY				1005
	#define ENT_CONTRAST			1006
	#define ENT_BRIGHTNESS			1007
	#define ENT_COLOR				1008
	#define ENT_AUTOCORRECT			1009
// Section "Archive"
#define SEC_NAME_ARCHIVE			"Archive"	
#define SEC_ARCHIVE				13
	#define ENT_ARCHIVNUMMER		1
	#define ENT_MAXARCHIVE			1000

// Section "SystemSettings"
#define SEC_NAME_SETTINGS		"Settings"	
#define SEC_SETTINGS			14
	#define ENT_NAME_STATIONID			"StationID"
	#define ENT_STATIONID				1
	#define ENT_OWNPICT_SHOW_INPUTLIST	2
	#define ENT_OWNPICT_SHOW_OUTPUTLIST	3
	#define ENT_OWNPICT_SHOW_CAMERANAME	4
	#define ENT_AUTO_LOGOUT_TIME		5


class CInifile : public CObject
{
public:
	// construction
	CInifile();

	// universal functions using strings for section and entry
	CString GetString(PCSTR szSection, PCSTR szEntry, PCSTR szDefault);
	BOOL WriteString(PCSTR szSection, PCSTR szEntry, PCSTR szValue);
	//	IntValue functions read/write INTEGER value, not strings!
	int GetIntValue(PCSTR szSection, PCSTR szEntry, int iDefault);
	BOOL WriteIntValue(PCSTR szSection, PCSTR szEntry, int iValue);

	// universal functions for "numbered" entries
	// using entry name %d from iEntry
	//	IntValue functions read/write INTEGER value, not strings!
	int GetIntValueByIndex(PCSTR szSection, int iEntry, int iDefault);
	BOOL WriteIntValueByIndex(PCSTR szSection, int iEntry, int iValue);
	CString GetStringByIndex(PCSTR szSection, int iEntry, PCSTR szDefault);
	BOOL WriteStringByIndex(PCSTR szSection, int iEntry, PCSTR szValue);
	
	//
	int  GetInt		(int iSection, int iEntry,	int iDefault);
	long GetLong	(int iSection, int iEntry,	long lDefault);
	BOOL GetString	(int iSection, int iEntry,	LPCSTR szDefault, LPSTR szBuffer, int iStrLen);
	BOOL GetString	(int iSection, LPCSTR szEntry, LPCSTR szDefault, LPSTR szBuffer, int iStrLen);

	BOOL WriteInt	(int iSection, int iEntry,	int iValue);
	BOOL WriteLong	(int iSection, int iEntry,	long lValue);
	BOOL WriteString(int iSection, int iEntry,	LPCSTR szBuffer);
	BOOL WriteString(int iSection, LPCSTR pEntry, LPCSTR szBuffer);

	//
	BOOL RemoveSubSection(const char *szSection, const char *szSubSection);
	void RemoveEntry(const char *szSection, const char *szEntry);
	void RemoveEntryByIndex(PCSTR szSection, int iEntry);

	// ChopSuey-functions for strings:
	PSTR GetStringFromString(PCSTR pString, PCSTR pFind, PSTR pBuffer, PSTR pDefault, int iLen);
	CString GetStringFromString(PCSTR pString, PCSTR pFind, PCSTR pDefault);
	long GetNrFromString(PCSTR pString, PCSTR pFind, long lDefault);
	long GetHexFromString(PCSTR pString, PCSTR pFind, long lDefault);

private:
	//static BOOL	m_bPfadOK;
	CString m_sSection;
	char m_szFilePath[_MAX_PATH];
	CString m_sEntry;

	void PrivateProfile(BOOL bRW, LPCSTR szDef, LPCSTR szString, int iStrLen);
	LPCSTR SwitchSection(BOOL bRW, int iSection, int iEntry, LPCSTR szDef, LPCSTR szString, int iStrLen);

};

#endif
