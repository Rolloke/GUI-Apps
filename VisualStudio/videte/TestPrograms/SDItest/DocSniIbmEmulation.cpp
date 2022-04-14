/* GlobalReplace: CSDITestSniIbmEmuDoc --> CDocSniIbmEmulation */
/* GlobalReplace: CSDITestIbmDoc --> CDocIbm */
// SDITestSniIbmEmuDoc.cpp: implementation of the CDocSniIbmEmulation class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DocSniIbmEmulation.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CDocSniIbmEmulation
IMPLEMENT_DYNCREATE(CDocSniIbmEmulation, CDocIbm)
/////////////////////////////////////////////////////////////////////////////
CDocSniIbmEmulation::CDocSniIbmEmulation() : CDocIbm()
{
	m_eType = SDICT_SNI_IBM_EMU;
}
//////////////////////////////////////////////////////////////////////
CDocSniIbmEmulation::~CDocSniIbmEmulation()
{
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDocSniIbmEmulation::OpenCom() 
{
	return m_pCom->Open(CBR_2400, 8, NOPARITY, ONESTOPBIT);
}
