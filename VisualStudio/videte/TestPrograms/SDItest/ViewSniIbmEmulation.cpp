/* GlobalReplace: CViewSniibmEmulation --> CViewIbmEmulation */
/* GlobalReplace: CSDITestSniIbmEmuView --> CViewIbmEmulation */
/* GlobalReplace: CSDITestSniView --> CViewSniDos */
/* GlobalReplace: CSDITestIbmView --> CViewIbm */
// SDITestSniIbmEmuView.cpp: implementation of the CViewIbmEmulation class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ViewSniIbmEmulation.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CViewSniDos
IMPLEMENT_DYNCREATE(CViewIbmEmulation, CViewIbm)
/////////////////////////////////////////////////////////////////////////////
CViewIbmEmulation::CViewIbmEmulation() : CViewIbm()
{
}
//////////////////////////////////////////////////////////////////////
CViewIbmEmulation::~CViewIbmEmulation()
{
}
