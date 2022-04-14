/////////////////////////////////////////////////////////////////////////////
// DlgLogin.cpp : implementation file
//
#include "stdafx.h"
#include "IdipClient.h"
#include "Mainfrm.h"
#include "ViewIdipClient.h"
#include ".\dlglogin.h"

#include <OemGui\OEMPasswordDialog.h>

/////////////////////////////////////////////////////////////////////////////
// CDlgLogin dialog
IMPLEMENT_DYNAMIC(CDlgLogin, CSkinDialog)
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CDlgLogin, CSkinDialog)
	ON_WM_NCDESTROY()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_MESSAGE(WM_USER_LOGIN_ALARM, OnUserLoginAlarm)
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
CDlgLogin::CDlgLogin(CWnd* pParent, 
					 DWORD dwPermissionFlags /*=0*/,
					 BOOL bDoubleLogin /*=FALSE*/,
					 BOOL bCenter /*=TRUE*/)
	: CSkinDialog(CDlgLogin::IDD, pParent)
{
	m_pMyOwnAndRealParent = pParent;
	m_nInitToolTips = FALSE;
	MoveOnClientAreaClick(FALSE);
	m_pSecondLogin = NULL;
	m_sUser = _T("");
	m_sPassword = _T("");
	m_pUser = NULL;
	m_pPermission = NULL;
	m_dwPermissionFlags = dwPermissionFlags;
	m_bSecondLogin = bDoubleLogin;
	m_bCenter = bCenter;
	if (UseOEMSkin())
	{
		CreateTransparent(StyleBackGroundPlaneColor, m_BaseColor);
		SetBaseColor(CSkinTree::GetOEMBkColor());
		if (m_bSecondLogin ||!m_bCenter)
		{
			SetBaseColor(SKIN_COLOR_RED);
		}
	}
	Create(IDD, pParent);
}
/////////////////////////////////////////////////////////////////////////////
CDlgLogin::~CDlgLogin()
{
	WK_DELETE(m_pUser);
	WK_DELETE(m_pPermission);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDlgLogin::StretchElements() 
{
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CDlgLogin::DoDataExchange(CDataExchange* pDX)
{
	CSkinDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_LOGIN_USER, m_sUser);
	DDX_Text(pDX, IDC_EDIT_LOGIN_PASSWORD, m_sPassword);
	DDX_Control(pDX, IDOK, m_btnLogin);
	DDX_Control(pDX, IDC_EDIT_LOGIN_USER, m_editUser);
	DDX_Control(pDX, IDC_EDIT_LOGIN_PASSWORD, m_editPassword);
	DDX_Control(pDX, IDC_TXT_LOGIN_PASSWORT, m_txtPassword);
	DDX_Control(pDX, IDC_TXT_LOGIN_USER, m_txtUser);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDlgLogin::OnInitDialog() 
{
	// We need the original size of the dlg eventually
	CRect rcDlg;
	GetWindowRect(&rcDlg);

	CSkinDialog::OnInitDialog();
//	AutoCreateSkinStatic();
//	AutoCreateSkinEdit();
//	AutoCreateSkinButtons();
//	KeepProportion(IDC_EDIT_LOGIN_USER, PROPORTION_CENTER_ALIGN);
//	KeepProportion(IDC_EDIT_LOGIN_PASSWORD, PROPORTION_CENTER_ALIGN);
//	KeepProportion(IDC_EDIT_LOGIN_USER, PROPORTION_CENTER_ALIGN);

	if (m_bSecondLogin)
	{
		CString sSecondLogin;
		sSecondLogin.LoadString(IDS_SECOND_LOGIN);
		m_btnLogin.SetWindowText(sSecondLogin);
		// Second Login is always centered above the first one
		CenterWindow();
		SetWindowPos(&wndTopMost, NULL, NULL, NULL, NULL, SWP_NOMOVE|SWP_NOSIZE);
	}
	else
	{
		SetWindowPosition();
	}

	ShowWindow(SW_SHOW);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
void CDlgLogin::OnOK() 
{
	CUser *pUser = CheckUserLogin();

	if (pUser)
	{
		// Doppelter Login notwendig?
		if (m_pUser->GetDualLogin() && !m_bSecondLogin)
		{
			m_txtUser.ShowWindow(SW_HIDE);
			m_editUser.EnableWindow(FALSE);
			m_editUser.ShowWindow(SW_HIDE);
			m_txtPassword.ShowWindow(SW_HIDE);
			m_editPassword.EnableWindow(FALSE);
			m_editPassword.ShowWindow(SW_HIDE);
			m_btnLogin.EnableWindow(FALSE);
			m_btnLogin.ShowWindow(SW_HIDE);
			m_pSecondLogin = new CDlgLogin(this, NULL, TRUE, TRUE);
		}
		else
		{
			if (m_bSecondLogin)
			{
				// 2mal der gleiche gilt nicht!
				if ( ((CDlgLogin*)GetParent())->GetUser().GetName() == pUser->GetName())
				{
					LoginFailed();
				}
				else
				{
					LoginOK();
				}
			}
			else
			{
				LoginOK();
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
CUser* CDlgLogin::CheckUserLogin()
{
	CSecID		idUser;
	CUserArray	UserArray;
	CWK_Profile	prof;
	CUser*		pUser	= NULL;
	BOOL bLoginSuccess	= FALSE;
	BOOL bSuperVisor	= FALSE;

	UpdateData();

	UserArray.Load(prof);		   

	// Ist der User geperrt?
	idUser = UserArray.IsValid(m_sUser);
	if (idUser.IsUserID())
	{
		pUser = UserArray.GetUser(idUser);
		if (pUser)
		{
			if (pUser->IsLocked())
			{
				UserIsLocked(pUser);
				return NULL;
			}
		}
	}
	else  // Diesen User gibt es nicht
	{
		LoginFailed();
		return NULL;
	}

	// Existiert User und Passwort
	idUser = UserArray.IsValid(m_sUser,m_sPassword);

	// Wenn User und Paßwort nicht passen, handelt es sich vielleicht um ein TempPaßwort?
	if (!idUser.IsUserID())
	{
		if (IsTempPassword())
			idUser = UserArray.IsValid(m_sUser);
	}

	if (idUser.IsUserID())
	{
		pUser = UserArray.GetUser(idUser);
		if (pUser)
		{
			CPermission* pPermission;
			CPermissionArray PermissionArray;

			PermissionArray.Load(prof);
			pPermission = PermissionArray.GetPermission(pUser->GetPermissionID());
			if (pPermission)
			{
				bSuperVisor = pPermission->IsSuperVisor();
// gf code commented out since creation
/*				if (   (pPermission->GetFlags()==0)
					&& !bSuperVisor
					)
				{
					if (   (pPermission->GetNumberOfArchivs()>0)
						|| (pPermission->GetNumberOfOutputs()>0)
						)
					{
						WK_DELETE(m_pPermission);
						m_pPermission = new CPermission(*pPermission);
						bLoginSuccess = TRUE;
					}
					else
					{
						CString sText;
						sText.LoadString(IDS_WRONG_PERMISSION);
						AfxMessageBox(sText, MB_OK);
						return NULL;
					}
				}
				else
*/				{
					if ((pPermission->GetFlags() & m_dwPermissionFlags) == m_dwPermissionFlags)
						//					if (pPermission->GetFlags() & m_dwPermissionFlags)
					{
						WK_DELETE(m_pPermission);
						m_pPermission = new CPermission(*pPermission);
						bLoginSuccess = TRUE;
					}						   
					else
					{
						CString sText;
						sText.LoadString(IDP_WRONG_PERMISSION);
						AfxMessageBox(sText, MB_OK);
						return NULL;
					}
				}
			}
		}
	}

	// Login erfolgreich?
	if (bLoginSuccess && !bSuperVisor)
	{
		// Ist das Paßwort abgelaufen? (Nicht prüfen für den SuperVisor)
		if (pUser->IsPasswordOutOfTime())
		{
			PasswordIsOutOfTime();
			bLoginSuccess = ChangePassword(pUser);
		}
	}

	if (bLoginSuccess)
	{
		pUser->LoginSuccess();
		WK_DELETE(m_pUser);
		m_pUser = new CUser(*pUser);
	}				    	 			 
	else			 			 
	{
		pUser->LoginFailed();
		LoginFailed();			  
	}

	UserArray.Save(prof);

	return (bLoginSuccess ? m_pUser : NULL);
}

////////////////////////////////////////////////////////////////////////////
BOOL CDlgLogin::IsTempPassword()
{
	if (m_sPassword.GetLength() != 8)
		return FALSE;

	// Mappingtabelle
	CString sMap		= _T("qwertzuiopasdfgh");
	DWORD dwTime		= 0;
	//	DWORD dwCurrTime 	= 0;
	CTime CurrTime		= CTime::GetCurrentTime();
	CTimeSpan Span;

	// Aus dem Paßwort ein DWORD bauen.
	for (int nI = 0; nI < m_sPassword.GetLength(); nI++)
	{
		char Val = (char)sMap.Find(m_sPassword[nI]);
		if (Val == -1)
			return FALSE;
		dwTime |= Val<<(4*(7-nI));
	}

	CTime PassTime( (time_t)dwTime);

	if (CurrTime >= PassTime)
		Span = CurrTime - PassTime;
	else
		Span = PassTime - CurrTime;

	// Paßwort ist maximal 1/2 Stunde gültig.
	LONGLONG diff = Span.GetTotalSeconds();
	return (0 < diff) && (diff < 1800);
}

////////////////////////////////////////////////////////////////////////////
LRESULT CDlgLogin::UserIsLocked(CUser *pUser)
{
	CString sText;
	sText.Format(IDS_USER_LOCKED, LPCTSTR(m_sUser), pUser->GetMaxTries(), pUser->GetLockTime());
	WK_TRACE_USER(LPCTSTR(sText));

	sText.Format(IDP_PASSWORD_LOCKED, pUser->GetMaxTries(), pUser->GetLockTime());
	LRESULT ret = AfxMessageBox(sText, MB_OK);

	m_editUser.SetFocus();

	return ret;
}

////////////////////////////////////////////////////////////////////////////
LRESULT CDlgLogin::LoginFailed()
{
	CString sText;
	sText.Format(IDS_LOGIN_FAILED, LPCTSTR(m_sUser));
	WK_TRACE_USER(_T("%s\n"), LPCTSTR (sText));

	sText.LoadString(IDP_LOGIN_FAILED);

	LRESULT ret = AfxMessageBox(sText, MB_ICONEXCLAMATION|MB_OK);

	m_sUser = "";
	m_sPassword = "";
	m_editUser.SetFocus();
	UpdateData(FALSE);

	return ret;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDlgLogin::PasswordIsOutOfTime()
{
	CString sText;
	sText.Format(IDS_PASSWORD_OLD, LPCTSTR(m_sUser));
	WK_TRACE_USER(LPCTSTR(sText));

	sText.LoadString(IDP_LOGIN_TIMEOUT);

	LRESULT ret = AfxMessageBox(sText, MB_ICONEXCLAMATION|MB_OK);

	return ret;

}

/////////////////////////////////////////////////////////////////////////////
CUser CDlgLogin::GetUser() const
{
	if (m_pUser)
	{
		return *m_pUser;	
	}
	else
	{
		return CUser();	
	}
}

/////////////////////////////////////////////////////////////////////////////
CPermission CDlgLogin::GetPermission() const
{
	if (m_pPermission)
	{
		return *m_pPermission;
	}
	else
	{
		return CPermission();
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDlgLogin::ChangePassword(CUser* pUser)
{
	COEMPasswordDialog dlg(this);
	BOOL bPasswordOK = FALSE;

	do {
		if (dlg.DoModal() != IDOK)
			break;

		if (dlg.GetPassword1() == pUser->GetPassword())
		{
			CString sText;
			sText.LoadString(IDP_PASSWORD_WRONG);
			AfxMessageBox(sText, MB_OK);
			bPasswordOK = FALSE;
		}
		else
		{
			pUser->SetPassword(dlg.GetPassword1());
			bPasswordOK = TRUE;
		}
	} while (!bPasswordOK);

	return bPasswordOK;
}
/////////////////////////////////////////////////////////////////////////////
void CDlgLogin::OnSize(UINT nType, int cx, int cy)
{
	// call the REAL base class for basic sizing
	CDialog::OnSize(nType, cx, cy);

	// now position the elements, us StrechElements features
	if (StretchElements())
	{
		// calculate difference in size
		CRect rect;
		GetClientRect(rect);
		int iDiff_X = (rect.Width()  - m_InitialClientRect.Width())  / 2;
		int iDiff_Y = (rect.Height() - m_InitialClientRect.Height()) / 2;

		CWnd* pItem = GetWindow(GW_CHILD);
		UINT uID = 0;
		CRect* pRect = NULL;
		while ( pItem && IsChild(pItem) ) 
		{
			uID = pItem->GetDlgCtrlID();
			if (m_RectMap.Lookup((WORD)uID, (void*&)pRect))
			{
				rect = *pRect;
				rect.left   += iDiff_X;
				rect.right  += iDiff_X;
				rect.top    += iDiff_Y;
				rect.bottom += iDiff_Y;

				//				TRACE(_T("lookup %d, %s %d,%d,%d,%d\n"),uID,text,
				//							rect.left,rect.top,rect.Width(),rect.Height());

				pItem->MoveWindow(rect);

			}

			// Haben wir noch ein Child?
			pItem = pItem->GetNextWindow();
		} // end of while

		GetClientRect(rect);
		InvalidateRect(rect);
	}
	// do not call base class 
	//	CSkinDialog::OnSize(nType, cx, cy);
}
/////////////////////////////////////////////////////////////////////////////
void CDlgLogin::OnDestroy()
{
	WK_DESTROY_WINDOW(m_pSecondLogin);
	CSkinDialog::OnDestroy();
}
/////////////////////////////////////////////////////////////////////////////
void CDlgLogin::OnNcDestroy()
{
	CSkinDialog::OnNcDestroy();

	delete this;
}
/////////////////////////////////////////////////////////////////////////////
void CDlgLogin::LoginOK()
{
	theApp.LoginOK();
}
/////////////////////////////////////////////////////////////////////////////
void CDlgLogin::OnCancel() 
{
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CDlgLogin::OnUserLoginAlarm(WPARAM wParam, LPARAM lParam)
{
	// use standard login
	CWK_Profile wkp;
	CUserArray users;
	users.Load(wkp);

	CString sPassword = _T("notfound");
	BOOL bFound=FALSE;
	for (int i=0 ; bFound==FALSE && i<users.GetSize() ; i++) 
	{
		const CUser &oneUser = *(users[i]);
		CString sName = oneUser.GetName();
		// TODO GF Implement check for user Alarm as function
		if (sName==_T("Alarm")) 
		{
			m_editUser.SetWindowText(sName);
			m_editPassword.SetWindowText(oneUser.GetPassword());	// sigh returns const char *
			bFound = TRUE;
		}
	}// end of loop over users

	if (bFound) 
	{
		OnOK();
	}
	else
	{
		WK_TRACE(_T("StandardAlarmUser not found, can't activate alarm client\n"));
	}
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CDlgLogin::SetWindowPosition() 
{
	if (m_pMyOwnAndRealParent)
	{
		CRect rcParent;
		m_pMyOwnAndRealParent->GetClientRect(&rcParent);
		if (m_bCenter)
		{
			SetWindowPos(NULL, rcParent.left, rcParent.top,
						 rcParent.Width(), rcParent.Height(),
						 SWP_NOZORDER);
		}
		else
		{
			CPoint pt = rcParent.TopLeft();
			LONG lLeft = rcParent.right;
			LONG lTop  = rcParent.bottom;
			lLeft -= m_InitialClientRect.Width();
			lTop  -= m_InitialClientRect.Height();
			if (m_pMyOwnAndRealParent->IsKindOf(RUNTIME_CLASS(CMainFrame)))
			{
	//			lTop  -= 20; // TODO GF Workaround to avoid the time control in DlgOem
			}
			SetWindowPos(&CWnd::wndTop,
				lLeft,
				lTop,
				NULL, NULL, SWP_NOZORDER|SWP_NOSIZE);
		}
	}
	CRect rect;
	GetClientRect(rect);
	InvalidateRect(rect);
}
