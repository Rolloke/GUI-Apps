// RecordAscom.h: interface for the CRecordAscom class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RECORDASCOM_H__38D82BF3_5C71_11D3_BA70_00400531137E__INCLUDED_)
#define AFX_RECORDASCOM_H__38D82BF3_5C71_11D3_BA70_00400531137E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "RecordIbm.h"

// Allgemeine Hinweise
//	Dieser GA liefert z.Z. UNBEKANNT(Annahme 16) Alarme

class CRecordAscom : public CRecordIbm  
{
public:
	CRecordAscom(SDIControlType type,
				  int iCom,
				  BOOL bTraceAscii,
				  BOOL bTraceHex,
				  BOOL bTraceEvents);
	virtual ~CRecordAscom();

// Overrides
public:
	virtual BOOL	Create(CWK_Profile& wkp, const CString& sSection);
};

#endif // !defined(AFX_RECORDASCOM_H__38D82BF3_5C71_11D3_BA70_00400531137E__INCLUDED_)
