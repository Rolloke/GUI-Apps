// RecordAscom.cpp: implementation of the CRecordAscom class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RecordAscom.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CRecordAscom::CRecordAscom(SDIControlType type,
							  int iCom,
							  BOOL bTraceAscii,
							  BOOL bTraceHex,
							  BOOL bTraceEvents)
	: CRecordIbm(type,
				  iCom,
				  bTraceAscii,
				  bTraceHex,
				  bTraceEvents)
{

}
/////////////////////////////////////////////////////////////////////////////
CRecordAscom::~CRecordAscom()
{
	Close();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CRecordAscom::Create(CWK_Profile& wkp, const CString& sSection)
{
	BOOL bReturn = FALSE;
	// Stimmt der Typ
	if (m_Type == SDICT_ASCOM) {
		// Basisklassenfunktion aufrufen
		if ( CreateRecord() ) {
			// RS232 oeffnen
			if ( OpenCom(CBR_1200, 7, ODDPARITY, ONESTOPBIT) ) {
				bReturn = TRUE;
			}
			else {
				WK_TRACE_ERROR(_T("%s Create OpenCom FAILED\n"),
								CSDIControl::NameOfEnum(m_Type));
			}
		}
		else {
			WK_TRACE_ERROR(_T("%s Create FAILED\n"), CSDIControl::NameOfEnum(m_Type));
		}
	}
	else {
		WK_TRACE_ERROR(_T("Create WRONG Type %s\n"), CSDIControl::NameOfEnum(m_Type));
	}
	return bReturn;
}
