/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: SDIMultiDocTemplate.cpp $
// ARCHIVE:		$Archive: /Project/Clients/SDIConfig/SDIMultiDocTemplate.cpp $
// CHECKIN:		$Date: 16.01.98 9:57 $
// VERSION:		$Revision: 3 $
// LAST CHANGE:	$Modtime: 5.12.97 13:21 $
// BY AUTHOR:	$Author: Georg $
// $Nokeywords:$

#include "stdafx.h"
#include "SDIConfig.h"
#include "SDIMultiDocTemplate.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSDIMultiDocTemplate
IMPLEMENT_DYNAMIC(CSDIMultiDocTemplate, CMultiDocTemplate)
/////////////////////////////////////////////////////////////////////////////
CSDIMultiDocTemplate::CSDIMultiDocTemplate(UINT nIDResource, CRuntimeClass* pDocClass,
	CRuntimeClass* pFrameClass, CRuntimeClass* pViewClass)
		: CMultiDocTemplate(nIDResource, pDocClass, pFrameClass, pViewClass)
{
}
/////////////////////////////////////////////////////////////////////////////
void CSDIMultiDocTemplate::SetDefaultTitle(CDocument* pDocument)
{
	CString strDocName;
	if (GetDocString(strDocName, CDocTemplate::docName) &&
		!strDocName.IsEmpty())
	{
// Hier tun wir mal gar nichts, um diese daemliche Numerierung zu vermeiden
//		TCHAR szNum[8];
//#ifndef _MAC
//		wsprintf(szNum, _T("%d"), m_nUntitledCount+1);
//#else
//		wsprintf(szNum, _T(" %d"), m_nUntitledCount+1);
//#endif
//		strDocName += szNum;
	}
	else
	{
		// use generic 'untitled' - ignore untitled count
		VERIFY(strDocName.LoadString(AFX_IDS_UNTITLED));
	}
	pDocument->SetTitle(strDocName);
}
