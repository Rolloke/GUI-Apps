/////////////////////////////////////////////////////////////////////////////
// PROJECT:		QUnit
// FILE:		$Workfile: CMDConfigDlgBase.cpp $
// ARCHIVE:		$Archive: /Project/Units/QUnit/CMDConfigDlgBase.cpp $
// CHECKIN:		$Date: 1.12.05 16:09 $
// VERSION:		$Revision: 5 $
// LAST CHANGE:	$Modtime: 1.12.05 14:58 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "CMDConfigDlgBase.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMDConfigDlgBase dialog


/////////////////////////////////////////////////////////////////////////////
CMDConfigDlgBase::CMDConfigDlgBase(CMotionDetection* pMD, CUDP* pUDP, CWnd* pParent, int nResourceID)
	: CDialog(nResourceID, pParent)
{
	//{{AFX_DATA_INIT(CMDConfigDlgBase)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pMD		= pMD;
	m_nCamera	= 0;
	m_pUDP		= pUDP;
	m_pMainWnd	= pParent;
	m_bVisible	= FALSE;
}

/////////////////////////////////////////////////////////////////////////////
void CMDConfigDlgBase::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMDConfigDlgBase)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CMDConfigDlgBase, CDialog)
	//{{AFX_MSG_MAP(CMDConfigDlgBase)
	ON_WM_CANCELMODE()
	ON_WM_CAPTURECHANGED()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
void CMDConfigDlgBase::OnCancelMode() 
{
	CDialog::OnCancelMode();
}

/////////////////////////////////////////////////////////////////////////////
void CMDConfigDlgBase::OnLButtonDown(UINT nFlags, CPoint point) 
{
	OnMouseMove(nFlags, point);
	CDialog::OnLButtonDown(nFlags, point);
}

/////////////////////////////////////////////////////////////////////////////
void CMDConfigDlgBase::OnRButtonDown(UINT nFlags, CPoint point) 
{
	OnMouseMove(nFlags, point);
	CDialog::OnRButtonDown(nFlags, point);
}

/////////////////////////////////////////////////////////////////////////////
void CMDConfigDlgBase::OnCaptureChanged(CWnd *pWnd) 
{
	CDialog::OnCaptureChanged(pWnd);
}

