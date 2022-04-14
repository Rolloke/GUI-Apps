/////////////////////////////////////////////////////////////////////////////
//	Project:	Akubis
//
//	File:		cdump.h : header file for cdump.cpp
//
//	Description:
//
//	Start:		01.06.95
//
//	Last change:21.07.95
//
//	Author:		Georg Feddern
//				Martin Lück
//
//	Copyright:	w+k Video Communication GmbH & Co.KG
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __cdump_h_
#define __cdump_h_

#define DEBUG_FILE		"DEBUG.TXT"
#define OLD_DEBUG_FILE	"DEBUG.OLD"
#define MAXFILELEN		100L

class CDump : public CObject
{
protected:
	BOOL	m_bFile;
	BOOL	m_bOK;
	CString	m_strFilename;
	CString	m_strOldFilename;
	LONG	m_lMaxFileLen;

public:
	CDump(BOOL bFile=FALSE);
	~CDump();

	BOOL			IsValid();
					// Ist das CDump-Objekt gültig?
	void __cdecl	Trace(LPCSTR lpszFormat, ...);
					// Funktionsaufruf wie TRACE
};

#endif // __cdump_h_