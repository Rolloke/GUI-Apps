/* GlobalReplace: CRecordSNI --> CRecordSniDos */
/* GlobalReplace: CRecordSniDos_IBM_EMU --> CRecordSniIbmEmulation */
/* GlobalReplace: CRecordIBM --> CRecordIbm */
/* GlobalReplace: SDIControlRecord_ --> Record */
// RecordSNI_IBM_EMU.h: interface for the CRecordSniIbmEmulation class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SDICONTROLRECORD_SNI_IBM_EMU_H__4BC9A263_CAEF_11D2_B07D_004005A1D890__INCLUDED_)
#define AFX_SDICONTROLRECORD_SNI_IBM_EMU_H__4BC9A263_CAEF_11D2_B07D_004005A1D890__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "RecordIBM.h"

class CRecordSniIbmEmulation : public CRecordIbm  
{
public:
	CRecordSniIbmEmulation(SDIControlType type,
								  int iCom,
								  BOOL bTraceAscii,
								  BOOL bTraceHex,
								  BOOL bTraceEvents);
	virtual ~CRecordSniIbmEmulation();

// Overrides
public:
	virtual BOOL	Create(CWK_Profile& wkp, const CString& sSection);
};

#endif // !defined(AFX_SDICONTROLRECORD_SNI_IBM_EMU_H__4BC9A263_CAEF_11D2_B07D_004005A1D890__INCLUDED_)
