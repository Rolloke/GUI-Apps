/* GlobalReplace: CRecordSNI --> CRecordSniDos */
/* GlobalReplace: CRecordSniDos_IBM_EMU --> CRecordSniIbmEmulation */
/* GlobalReplace: CRecordIBM --> CRecordIbm */
/* GlobalReplace: SDIControlRecord_ --> Record */
// RecordSNI_IBM_EMU.cpp: implementation of the CRecordSniIbmEmulation class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RecordSniIbmEmulation.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//////////////////////////////////////////////////////////////////////
CRecordSniIbmEmulation::CRecordSniIbmEmulation(SDIControlType type,
															 int iCom,
															 BOOL bTraceAscii,
															 BOOL bTraceHex,
															 BOOL bTraceEvents)
	: CRecordIbm(type, iCom, bTraceAscii, bTraceHex, bTraceEvents)
{
}
/////////////////////////////////////////////////////////////////////////////
CRecordSniIbmEmulation::~CRecordSniIbmEmulation()
{
	Close();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CRecordSniIbmEmulation::Create(CWK_Profile& wkp, const CString& sSection)
{
	BOOL bReturn = FALSE;
	// Stimmt der Typ
	if (m_Type == SDICT_SNI_IBM_EMU) {
		// Basisklassenfunktion aufrufen
		if ( CreateRecord() ) {
			// RS232 oeffnen
			if ( OpenCom(CBR_2400, 8, NOPARITY, ONESTOPBIT) ) {
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
