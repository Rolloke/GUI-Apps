// stdafx.cpp : source file that includes just the standard includes
//	Vision.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"
#include "resource.h"

void DDV_MinChars(CDataExchange* pDX, CString const& value, int nChars, int nID)
{
	if (   pDX->m_bSaveAndValidate 
		 && value.GetLength() <= nChars)
	{
		CString s, sText;
		CWnd *pWnd = NULL;
		if (nID)  pWnd = pDX->m_pDlgWnd->GetDlgItem(nID);
		if (pWnd) pWnd->GetWindowText(sText);
		else      sText = _T("");

		s.Format(IDS_DO_NOT_LEAVE_EMPTY, sText);
		AfxMessageBox(s);
		pDX->Fail();
	}
}
//////////////////////////////////////////////////////////////////////////
