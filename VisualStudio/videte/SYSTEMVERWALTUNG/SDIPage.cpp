// SDIPage.cpp : implementation file
//

#include "stdafx.h"
#include "systemverwaltung.h"
#include "SDIPage.h"

#include "SVDoc.h"
#include "SVView.h"

#include "Input.h"
#include "InputList.h"
#include "InputGroup.h"
#include "InputToOutput.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
#define MAX_DLG_ON_PAGE 12

#define DEVICE_IDENTIFIER _T("SDI Inputs")

/////////////////////////////////////////////////////////////////////////////
// CSDIPage dialog
CSDIPage::CSDIPage(CSVView* pParent) : CSVPage(CSDIPage::IDD)
{
	m_pParent = pParent;
	m_pInputList    = pParent->GetDocument()->GetInputs();
	m_pDatabaseFields = pParent->GetDocument()->GetDatabaseFields();
	//{{AFX_DATA_INIT(CSDIPage)
	m_iScroll = 0;
	//}}AFX_DATA_INIT
	Create(IDD,pParent);
}
/////////////////////////////////////////////////////////////////////////////
void CSDIPage::DoDataExchange(CDataExchange* pDX)
{
	CSVPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSDIPage)
	DDX_Control(pDX, IDC_TXT_COM, m_staticCom);
	DDX_Control(pDX, IDC_SCROLLBAR, m_ScrollBar);
	DDX_Scroll(pDX, IDC_SCROLLBAR, m_iScroll);
	//}}AFX_DATA_MAP
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CSDIPage, CSVPage)
	//{{AFX_MSG_MAP(CSDIPage)
	ON_WM_SIZE()
	ON_WM_VSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
////////////////////////////////////////////////////////////////////////////
BOOL CSDIPage::StretchElements()
{
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSDIPage::IsDataValid()
{
	BOOL bReturn = TRUE;

	CPtrList lPorts;
	CSDIDialog* pSDIDialog = NULL;
	int i, c = m_SDIDialogs.GetSize();
	for (i=0; i<c; i++)
	{
		// Sind die Daten des einzelnen Dialoges korrekt
		pSDIDialog = m_SDIDialogs.GetAt(i);
		bReturn = pSDIDialog->IsDataValid();
		if (bReturn && pSDIDialog->GetInterface() == SDI_INTERFACE_UDP)
		{
			if (lPorts.Find((void*)pSDIDialog->GetPortNo()))
			{
				bReturn = FALSE;
				CString sFmt, sType;
				pSDIDialog->GetDlgItem(IDC_COM_SDI)->GetWindowText(sType);
				sFmt.Format(IDP_SDI_DATA_TYPE_DOUBLE, sType);
				AfxMessageBox(sFmt, MB_ICONSTOP);
				pSDIDialog->GetDlgItem(IDC_COM_SDI)->SetFocus();
			}
			else
			{
				lPorts.AddTail((void*)pSDIDialog->GetPortNo());
			}
		}
		if (!bReturn)
		{
			break;
		}
	}

	if (   bReturn == FALSE 
		&& c > MAX_DLG_ON_PAGE)
	{
		if (i<m_iScroll)
		{
			m_iScroll = i;
			UpdateData(FALSE);
			PostResize();
		}
		else if (i >= m_iScroll+MAX_DLG_ON_PAGE)
		{
			m_iScroll = i - MAX_DLG_ON_PAGE + 1;
			UpdateData(FALSE);
			PostResize();
		}
	}
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
void CSDIPage::SaveChanges()
{
	WK_TRACE_USER(_T("InterfaceSerialData-Einstellungen wurden geändert\n"));
	UpdateData();

	CSDIDialog* pSDIDialog = NULL;
	int i, c, nR, nNoOfUDP = 0;
	BOOL bOn = FALSE;
	c = m_SDIDialogs.GetSize();
	for (i=0; i<c; i++)
	{
		pSDIDialog = m_SDIDialogs.GetAtFast(i);
		bOn |= pSDIDialog->IsOn();
		if (pSDIDialog->GetInterface() == SDI_INTERFACE_UDP)
		{
			nNoOfUDP++;
		}
		else if (pSDIDialog->GetInterface() == SDI_INTERFACE_COM)
		{
			SetCommPortUsed(GetProfile(), REMOVE_DEVICE, DEVICE_IDENTIFIER);
		}
	}

	GetProfile().WriteString(theApp.GetModuleSection(),WK_APP_NAME_SDIUNIT,bOn?_T("SDIUnit.exe"):_T(""));
	GetProfile().WriteInt(szSectionSDI, szEntryNoOfUDP, nNoOfUDP);

	int nFirstUDP = 1, nFirstTCP = 1;
	for (i=0; i<c; i++)
	{
		pSDIDialog = m_SDIDialogs.GetAtFast(i);
		switch (pSDIDialog->GetInterface())
		{
			case SDI_INTERFACE_COM: 
				if (pSDIDialog->IsOn())
				{
					SetCommPortUsed(GetProfile(), pSDIDialog->GetPortNo(), DEVICE_IDENTIFIER); 
				}
				break;
			case SDI_INTERFACE_UDP: 
				pSDIDialog->SetLocationNo(nFirstUDP++);   
				break;
			case SDI_INTERFACE_TCP: 
				pSDIDialog->SetLocationNo(nFirstTCP++); 
				break;
		}
		pSDIDialog->SaveChanges();
	}

	nR = m_Removed.GetSize();
	for (i=0; i<nR; i++)
	{
		pSDIDialog = m_Removed.GetAtFast(i);
		switch (pSDIDialog->GetInterface())
		{
			case SDI_INTERFACE_UDP: pSDIDialog->SetLocationNo(nFirstUDP++);   break;
			case SDI_INTERFACE_TCP: pSDIDialog->SetLocationNo(nFirstTCP++); break;
		}
		pSDIDialog->SaveChanges();
		pSDIDialog->DestroyWindow();
	}
	m_Removed.RemoveAll();

	m_pDatabaseFields->Save(SECTION_DATABASE_FIELDS,GetProfile());
	m_pInputList->Save(GetProfile(),IsLocal());
	SetModified(FALSE,FALSE);
	m_pParent->PostMessage(WM_USER, USER_MSG_UPDATE_PARENT_TREE, 1<<IMAGE_INPUT);
}
/////////////////////////////////////////////////////////////////////////////
void CSDIPage::CancelChanges()
{
	CString sT;

	sT = GetProfile().GetString(theApp.GetModuleSection(),WK_APP_NAME_SDIUNIT,_T(""));

	CSDIDialog* pSDIDialog;
	int i,c, nR;
	nR = m_Removed.GetSize();
	for (i=0; i<nR; i++)
	{
		pSDIDialog = m_Removed.GetAtFast(i);
		pSDIDialog->SwitchOn(TRUE);
		m_SDIDialogs.Add(pSDIDialog);
	}
	m_Removed.RemoveAll();

	c = m_SDIDialogs.GetSize();
	for (i=0; i<c; i++)
	{
		pSDIDialog = m_SDIDialogs.GetAt(i);
		pSDIDialog->CancelChanges();
		if (pSDIDialog->GetPortNo() == 0)
		{
			m_Removed.Add(pSDIDialog);
			nR++;
		}
	}
	c = m_Removed.GetSize();
	for (i=0; i<c; i++)
	{
		pSDIDialog = m_Removed.GetAtFast(i);
		m_SDIDialogs.Remove(pSDIDialog);
		pSDIDialog->DestroyWindow();
	}
	m_Removed.RemoveAll();

	if (nR)
	{
		PostResize();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSDIPage::OnNew()
{
	CSDIDialog* pSDIDialog = new CSDIDialog(m_pParent, this, 0, SDI_INTERFACE_UDP);
	pSDIDialog->CancelChanges();
	m_SDIDialogs.Add(pSDIDialog);
	SetModified();
	PostResize();
}
/////////////////////////////////////////////////////////////////////////////
void CSDIPage::OnDelete()
{
	CWnd *pWnd = GetFocus();
	if (pWnd && pWnd->GetDlgCtrlID() == IDC_COM_SDI)
	{
		pWnd = pWnd->GetParent();
		int i, c = m_SDIDialogs.GetSize();
		for (i=0; i<c; i++)
		{
			CSDIDialog* pSDIDialog = m_SDIDialogs.GetAtFast(i);
			if (pWnd == pSDIDialog)
			{
				pSDIDialog->SwitchOn(FALSE);
				m_SDIDialogs.Remove(pSDIDialog);
				pSDIDialog->ShowWindow(SW_HIDE);
				m_Removed.Add(pSDIDialog);
				PostResize();
				SetModified();
				break;
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSDIPage::CanNew()
{
	return GetDongle().AllowSDIParkmanagment();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSDIPage::CanDelete()
{
	CWnd* pWnd = GetFocus();
	if (   pWnd 
		&& pWnd->GetDlgCtrlID() == IDC_COM_SDI)
	{
		return GetDongle().AllowSDIParkmanagment();
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
void CSDIPage::CreateSDIDialogs()
{
	CSDIDialog* pSDIDialog;
	CString sDevice;
	BOOL bCanBeUsed = TRUE;
	DWORD dwComMask = m_pParent->GetDocument()->GetCommPortInfo();
	DWORD dwBit;
	int i;
	for (dwBit = 1,i=1; dwBit && dwBit<=dwComMask; dwBit<<=1,i++)
	{
		if (dwComMask & dwBit)
		{
			bCanBeUsed = TRUE;
			sDevice = IsCommPortUsed(GetProfile(), i);
			if (!sDevice.IsEmpty() && sDevice != DEVICE_IDENTIFIER)
			{
				bCanBeUsed = FALSE;
			}

			// Nur eintragen, wenn nicht bereits belegt
			if (bCanBeUsed)
			{
				pSDIDialog = new CSDIDialog(m_pParent, this, i);
				m_SDIDialogs.Add(pSDIDialog);
			}
		}
	}

	int nNoOfUDP = GetProfile().GetInt(szSectionSDI, szEntryNoOfUDP, 0);
	for (i=1; i<=nNoOfUDP; i++)
	{
		pSDIDialog = new CSDIDialog(m_pParent, this, i, SDI_INTERFACE_UDP);
		m_SDIDialogs.Add(pSDIDialog);
	}
	Resize();
}
/////////////////////////////////////////////////////////////////////////////
void CSDIPage::PostResize()
{
	CRect rc;
	GetClientRect(&rc);
	PostMessage(WM_SIZE, 0, MAKELONG(rc.Width(), rc.Height()));
}
/////////////////////////////////////////////////////////////////////////////
void CSDIPage::Resize()
{
	if (m_staticCom.m_hWnd)
	{
		CRect rect;
		CSDIDialog* pSDIDialog;
		int i,c,nDlgHeight,nOffset, nLastVisible, nSBleftPos= 0;

		c = m_SDIDialogs.GetSize();
		if (c > MAX_DLG_ON_PAGE)
		{
			m_ScrollBar.ShowWindow(SW_SHOW);
			m_ScrollBar.SetScrollRange(0, c-1);
			SetPageSize(m_ScrollBar, MAX_DLG_ON_PAGE);
			m_ScrollBar.GetWindowRect(&rect);
			ScreenToClient(&rect);
			nSBleftPos = rect.left-2;
			i=m_iScroll;
			nLastVisible = i + GetPageSize(m_ScrollBar);
			if (nLastVisible > c) nLastVisible = c;
		}
		else
		{
			m_ScrollBar.ShowWindow(SW_HIDE);
			i = 0;
			m_iScroll = 0;
			nLastVisible = c;
		}

		m_staticCom.GetWindowRect(rect);
		ScreenToClient(rect);
		nOffset = rect.bottom;

		if (c>0)
		{
			pSDIDialog = m_SDIDialogs.GetAt(i);
			pSDIDialog->GetClientRect(rect);
			nDlgHeight = rect.Height();
			if (nSBleftPos)
			{
				rect.right = nSBleftPos;
			}
			rect.OffsetRect(0, nOffset);
			
			for (;i<nLastVisible;i++)
			{
				pSDIDialog = m_SDIDialogs.GetAt(i);
				pSDIDialog->MoveWindow(rect, FALSE);
				pSDIDialog->ShowWindow(SW_SHOW);
				rect.OffsetRect(0, nDlgHeight);
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSDIPage::OnInitDialog() 
{
	SetDlgItemText(IDC_TXT_SDI,COemGuiApi::GetApplicationNameOem(WAI_SDIUNIT));
	CreateSDIDialogs();

	CSVPage::OnInitDialog();

	CancelChanges();
	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
//////////////////////////////////////////////////////////////////////////
void CSDIPage::OnSize(UINT nType, int cx, int cy) 
{
	int i, c = m_SDIDialogs.GetSize();
	for (i=0;i<c;i++)
	{
		m_SDIDialogs.GetAt(i)->ShowWindow(SW_HIDE);
	}
	CSVPage::OnSize(nType, cx, cy);
	Resize();
}
//////////////////////////////////////////////////////////////////////////
void CSDIPage::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	int oldScroll = m_iScroll;
	CSVPage::OnVScroll(m_ScrollBar, nSBCode, nPos, m_SDIDialogs.GetSize(), m_iScroll);

	if (oldScroll != m_iScroll)
	{
		UpdateData(FALSE);
		PostResize();
	}

	CWK_Dialog::OnVScroll(nSBCode, nPos, pScrollBar);
}
