// ErrorDialog.cpp : implementation file
//

#include "stdafx.h"
#include "securitylauncher.h"
#include "ErrorDialog.h"

#include "Application.h"
#include "WatchDog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CSecurityLauncherApp theApp;


/////////////////////////////////////////////////////////////////////////////
// CErrorDialog dialog


CErrorDialog::CErrorDialog(CWnd* pParent /*=NULL*/)
	: CSkinDialog(CErrorDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CErrorDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_nInitToolTips = TOOLTIPS_SIMPLE;
	Create(IDD,pParent);
}
////////////////////////////////////////////////////////////////////////////
BOOL CErrorDialog::StretchElements()
{
	return TRUE;
}


void CErrorDialog::DoDataExchange(CDataExchange* pDX)
{
	CSkinDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CErrorDialog)
	DDX_Control(pDX, IDC_DELETE, m_btnDelete);
	DDX_Control(pDX, IDC_ERRORS, m_lErrors);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CErrorDialog, CSkinDialog)
	//{{AFX_MSG_MAP(CErrorDialog)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_STMOFF, OnStmoff)
	ON_NOTIFY(NM_DBLCLK, IDC_ERRORS, OnDblclkErrors)
	ON_BN_CLICKED(IDC_STMON, OnStmon)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_ERRORS, OnItemchangedErrors)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CErrorDialog::PostNcDestroy() 
{
	theApp.m_pErrorDialog = NULL;
	delete this;
}
/////////////////////////////////////////////////////////////////////////////
// CErrorDialog message handlers
void CErrorDialog::OnOK() 
{
	DestroyWindow();
}
/////////////////////////////////////////////////////////////////////////////
void CErrorDialog::OnCancel() 
{
	DestroyWindow();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CErrorDialog::OnInitDialog() 
{
	CSkinDialog::OnInitDialog();
	AutoCreateSkinButtons();

	m_Images.Create(IDB_ERROR_SYMBOLS,16,0,RGB(0,255,255));
	
	const int		iColumns = 4;
	CRect			rect;
	LV_COLUMN		lvcolumn;
	CString			s[iColumns];
	int i;
	int w[] = {20,15,45,20};

	// Activation ListCtrl initialisieren
	m_lErrors.GetClientRect(&rect);
	s[0].LoadString(IDS_TIME);
	s[1].LoadString(IDS_MODUL);
	s[2].LoadString(IDS_MESSAGE);
	s[3].LoadString(IDS_PARAMETER);

	lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	lvcolumn.fmt = LVCFMT_LEFT;

	for (i = 0 ; i < iColumns ; i++)  // add the columns to the list control
	{
		lvcolumn.cx = (rect.Width() * w[i])/100;
		lvcolumn.pszText = s[i].GetBuffer(0);
		lvcolumn.iSubItem = i;
		m_lErrors.InsertColumn(i, &lvcolumn);  // assumes return value is OK.
		s[i].ReleaseBuffer();
	}

	LONG dw = ListView_GetExtendedListViewStyle(m_lErrors.m_hWnd);
	dw |= LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES;
	ListView_SetExtendedListViewStyle(m_lErrors.m_hWnd,dw);

	m_lErrors.SetImageList(&m_Images,LVSIL_SMALL);

	m_btnDelete.EnableWindow(m_lErrors.GetSelectedCount());

	// Die Störmelder gibt es nur wenn entweder MiCo, JaCob oder CoCo vorhanden sind
	if (!WK_IS_RUNNING(WK_APP_NAME_MICOUNIT)	&&
		!WK_IS_RUNNING(WK_APP_NAME_JACOBUNIT1)	&&
		!WK_IS_RUNNING(WK_APP_NAME_SAVICUNIT1)	&&
		!WK_IS_RUNNING(WK_APP_NAME_TASHAUNIT1)	&&
		!WK_IS_RUNNING(WK_APP_NAME_COCOUNIT))
	{
		CWnd* pWnd = GetDlgItem(IDC_STMON);
		if (pWnd)
			pWnd->ShowWindow(SW_HIDE);
		pWnd = GetDlgItem(IDC_STMOFF);
		if (pWnd)
			pWnd->ShowWindow(SW_HIDE);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
void CErrorDialog::InsertErrors(CWK_RunTimeErrors* pErrors)
{
	int i,c;

	m_lErrors.DeleteAllItems();
	c = pErrors->GetSize();
	for (i=0;i<c;i++)
	{
		InsertError(pErrors->GetAt(i));
	}
}
/////////////////////////////////////////////////////////////////////////////
void CErrorDialog::InsertError(CWK_RunTimeError* pError)
{
	if (!theApp.DoShowError(pError))
	{
		return ;
	}
	LV_ITEM lvis;
	CString s0,s1,s2,s3;
	int iIndex = -1;

	s0 = pError->GetFormattedTimeStamp();

	// Item einfuegen
	// Zeit
	lvis.mask = LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE;
	lvis.pszText = s0.GetBuffer(0);
	lvis.cchTextMax = s0.GetLength();
	lvis.lParam = (LPARAM)pError;
	lvis.iItem = 0;
	lvis.iSubItem = 0;
	switch(pError->GetLevel())
	{
	case REL_REBOOT_ERROR:
	case REL_RESTART_ME:
	case REL_CANNOT_RUN:
		lvis.iImage = 0;
		break;
	case REL_ERROR:
		lvis.iImage = 1;
		break;
	case REL_WARNING:
		lvis.iImage = 2;
		break;
	case REL_MESSAGE:
		lvis.iImage = 3; 
		break;
	default:
		lvis.iImage = 2;
	}

	// ML Dies war der Grund, warum manchmal leere RTE hochpopten!
	// if (lvis.iImage != -1)
	{
		iIndex = m_lErrors.InsertItem(&lvis);
	}

	s0.ReleaseBuffer();

	// Modul
	if (pError->GetApplicationId()==WAI_LAUNCHER)
	{
		s1 = theApp.GetName();
	}
	else
	{
		CApplication* pAR = theApp.m_Apps.GetApplication(pError->GetApplicationId());
		if (pAR)
		{
			s1 = pAR->GetName();
		}
		else
		{
			s1 = GetAppnameFromId(pError->GetApplicationId());
			// s1.LoadString(IDS_UNKOWN);
		}
	}
	lvis.mask = LVIF_TEXT;
	lvis.pszText = s1.GetBuffer(0);
	lvis.cchTextMax = s1.GetLength();
	lvis.iItem = iIndex;
	lvis.iSubItem = 1;
	m_lErrors.SetItem(&lvis);
	s1.ReleaseBuffer();

	// Fehler
	s2 = pError->GetErrorText();
	int i,c;
	c = s2.GetLength();
	for (i=0;i<c;i++)
	{
		if ( (s2[i]==_T('\r')) || (s2[i]==_T('\n')) || (s2[i]==_T('\t')))
		{
			s2.SetAt(i,_T(' '));
		}
	}

	lvis.mask = LVIF_TEXT;
	lvis.pszText = s2.GetBuffer(0);
	lvis.cchTextMax = s2.GetLength();
	lvis.iItem = iIndex;
	lvis.iSubItem = 2;
	m_lErrors.SetItem(&lvis);
	s2.ReleaseBuffer();

	CString sT;
	if (pError->GetParam1()!=0)
	{
		sT.Format(_T("%d"),pError->GetParam1());
		s3 += sT;
	}
	if (pError->GetParam2()!=0)
	{
		sT.Format(_T("%d"),pError->GetParam2());
		s3 += _T(',') + sT;
	}
	if (pError->GetSubcode()!=0)
	{
		sT.Format(_T("%d"),pError->GetSubcode());
		s3 += _T(',') + sT;
	}

	lvis.mask = LVIF_TEXT;
	lvis.pszText = s3.GetBuffer(0);
	lvis.cchTextMax = s3.GetLength();
	lvis.iItem = iIndex;
	lvis.iSubItem = 3;
	m_lErrors.SetItem(&lvis);
	s3.ReleaseBuffer();

	m_btnDelete.EnableWindow(m_lErrors.GetSelectedCount());

}
/////////////////////////////////////////////////////////////////////////////
void CErrorDialog::OnDelete() 
{
	int i,c,s;

	c = m_lErrors.GetItemCount();
	s = m_lErrors.GetSelectedCount();
	if (c && s)
	{
		if (theApp.Login())
		{
			c = m_lErrors.GetItemCount();
			for (i=c-1;i>=0;i--)
			{
				if (m_lErrors.GetItemState(i,LVIS_SELECTED)==LVIS_SELECTED)
				{
					CWK_RunTimeError* pError = (CWK_RunTimeError*)m_lErrors.GetItemData(i);
					m_lErrors.DeleteItem(i);
					theApp.m_Errors.Lock();
					theApp.m_Errors.Remove(pError);
					theApp.m_Errors.Unlock();
				}
			}
			theApp.m_Errors.Lock();
			theApp.m_Errors.WriteToFile(theApp.GetErrorDat());
			theApp.m_Errors.Unlock();
		}
	}
	m_btnDelete.EnableWindow(m_lErrors.GetSelectedCount());
}
/////////////////////////////////////////////////////////////////////////////
void CErrorDialog::OnStmoff() 
{
	theApp.m_WatchDog.PiezoSetFlag(0);
}

void CErrorDialog::OnDblclkErrors(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CString sMessage;

	int i,c;
	c = m_lErrors.GetItemCount();
	for (i=c-1;i>=0;i--)
	{
		if (m_lErrors.GetItemState(i,LVIS_SELECTED)==LVIS_SELECTED)
		{
			CWK_RunTimeError* pError = (CWK_RunTimeError*)m_lErrors.GetItemData(i);
			if (pError)
			{
				CString sT,sM;
				sMessage = pError->GetErrorText();
				sMessage += _T('\n');
				sT.LoadString(IDS_TIME);
				sMessage += sT + _T(' ');
				sMessage += pError->GetFormattedTimeStamp();
				sMessage += _T('\n');
				sT.LoadString(IDS_MODUL);
				sMessage += sT + _T(' ');
				if (pError->GetApplicationId()==WAI_LAUNCHER)
				{
					sM = theApp.GetName();
				}
				else
				{
					CApplication* pAR = theApp.m_Apps.GetApplication(pError->GetApplicationId());
					if (pAR)
					{
						sM = pAR->GetName();
					}
					else
					{
						sM = GetAppnameFromId(pError->GetApplicationId());
					}
				}
				sMessage += sM;
				if (pError->GetParam1()+pError->GetParam2()+pError->GetSubcode()>0)
				{
					sMessage += _T('\n');
					sT.LoadString(IDS_PARAMETER);
					sMessage += sT + _T(' ');
					if (pError->GetParam1()!=0)
					{
						sT.Format(_T("%d"),pError->GetParam1());
						sMessage += sT;
					}
					if (pError->GetParam2()!=0)
					{
						sT.Format(_T("%d"),pError->GetParam2());
						sMessage += _T(',') + sT;
					}
					if (pError->GetSubcode()!=0)
					{
						sT.Format(_T("%d"),pError->GetSubcode());
						sMessage += _T(',') + sT;
					}
				}
				AfxMessageBox(sMessage, MB_ICONEXCLAMATION|MB_OK);
//				AfxGetMainWnd()->MessageBox(sMessage,theApp.GetName(),MB_ICONEXCLAMATION|MB_OK);
				break;
			}
		}
	}
	m_btnDelete.EnableWindow(m_lErrors.GetSelectedCount());
	
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CErrorDialog::OnStmon() 
{
	theApp.m_WatchDog.PiezoSetFlag(PF_STM_MANUAL);
}

void CErrorDialog::OnItemchangedErrors(NMHDR* pNMHDR, LRESULT* pResult) 
{
//	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	m_btnDelete.EnableWindow(m_lErrors.GetSelectedCount());

	*pResult = 0;
}
