// stdafx.cpp : Quelltextdatei, die nur die Standard-Includes einbindet
//  IdipClient.pch ist der vorcompilierte Header
//  stdafx.obj enthält die vorcompilierte Typinformation

#include "stdafx.h"
#include "resource.h"

CStateCmdUI::CStateCmdUI()
{
	m_bEnabled = FALSE;
	m_nChecked = 0;
}

void CStateCmdUI::Enable(BOOL bOn)
{
	m_bEnabled = bOn;
}

void CStateCmdUI::SetCheck(int nC)
{
	m_nChecked = nC;
}

void CStateCmdUI::SetRadio( BOOL bOn)
{
	if (bOn)
	{
		m_nChecked = m_nID;
	}
}

void DDV_MinChars(CDataExchange* pDX, CString const& value, int nChars, int nID)
{
	if (   pDX->m_bSaveAndValidate 
		 && value.GetLength() <= nChars)
	{
		CString s, sText, sField;
		CWnd *pWnd = NULL;
		if (nID)  pWnd = pDX->m_pDlgWnd->GetDlgItem(nID);
		if (pWnd) pWnd->GetWindowText(sText);
		else      sText = _T("");

		sField.Format(_T("`%s`"), sText);

		s.Format(IDS_DO_NOT_LEAVE_EMPTY, sField);
		AfxMessageBox(s);
		pDX->Fail();
	}
}

int GetPosition(CDWordArray& array, DWORD dwValue)
{
	int i;
	for (i=0;i<array.GetSize();i++)
	{
		if (array[i]==dwValue)
		{
			return i;
		}
	}
	return -1;
}

void AddSorted(CDWordArray& array, DWORD dwValue)
{
	int i=0;
	BOOL bInsert = TRUE;
	for (i=0;i<array.GetSize();i++)
	{
		if (array[i]<dwValue)
		{
			break;
		}
		if (array[i]==dwValue)
		{
			bInsert = FALSE;
			break;
		}
	}
	if (bInsert)
	{
		if (i>=0)
		{
			array.InsertAt(i,dwValue);
		}
		else
		{
			array.Add(dwValue);
		}
	}
}

int CompareStringRelative(const CString &s1, const CString &s2)
{
	int i, n=0, n1 = s1.GetLength(), n2 = s2.GetLength();
	if (n1 != n2)
	{
		return 0;
	}
	for (i=0; i<n1; i++)
	{
		if (s1[i] == s2[i])
		{
			n++;
		}
	}
	return MulDiv(100, n, n1); 
}
