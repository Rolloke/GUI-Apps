// SecurityPage.cpp : implementation file
//

#include "stdafx.h"
#include "systemverwaltung.h"
#include "SecurityPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
static TCHAR BASED_CODE szSecurityLevel[] = _T("SecurityLevel");

/////////////////////////////////////////////////////////////////////////////
// CSecurityPage dialog
CSecurityPage::CSecurityPage(CSVView* pParent) : CSVPage(CSecurityPage::IDD)
{
	m_pParent		= pParent;
	m_bLoadingTPWL	= FALSE;

	//{{AFX_DATA_INIT(CSecurityPage)
	m_dwMinPasswordLength	= WK_PASSWORD_MIN_LEN;		// Mindestlänge
	m_dwMinPasswordAlpha	= WK_PASSWORD_MIN_ALPHA;	// Mindestanzahl der Buchstaben
	m_dwMinPasswordNum		= WK_PASSWORD_MIN_NUM;		// Mindestanzahl der Ziffern
	m_dwTimeLimit			= 0;	// Gültikeitsdauer in Tagen
	m_dwMaxLoginTries		= 3;	// Anzahl der erlaubten Fehlversuche
	m_dwUserLockTime		= 24;	// Sperrzeit in Stunden
	m_bUseTPWL = FALSE;
	m_sTrivialPasswordList = _T("");
	//}}AFX_DATA_INIT
	Create(IDD,(CWnd*)pParent);
}
/////////////////////////////////////////////////////////////////////////////
// Overrides
/////////////////////////////////////////////////////////////////////////////
BOOL CSecurityPage::IsDataValid()
{
	UpdateData();

	CString sMsg;
	if (m_dwMinPasswordLength > WK_PASSWORDLEN)
	{
		sMsg.Format(IDP_PASSWORD_TOO_LONG, WK_PASSWORDLEN);
		AfxMessageBox(sMsg, MB_OK);					  
		m_editMinPasswordLength.SetFocus();
		m_editMinPasswordLength.SetSel(0, -1);
		return FALSE;
	}

	if (m_dwMinPasswordAlpha + m_dwMinPasswordNum > WK_PASSWORDLEN)
	{
		sMsg.Format(IDP_PASSWORD_ALPHANUM_TOO_LONG, WK_PASSWORDLEN);
		AfxMessageBox(sMsg, MB_OK);					  
		m_editMinPasswordAlpha.SetFocus();
		m_editMinPasswordAlpha.SetSel(0, -1);
		return FALSE;
	}

	if (m_dwMinPasswordAlpha + m_dwMinPasswordNum > m_dwMinPasswordLength)
	{
		sMsg.Format(IDP_PASSWORD_ALPHANUM_LONGER_THAN_PASSLEN, WK_PASSWORDLEN);
		AfxMessageBox(sMsg, MB_OK);					  
		m_dwMinPasswordLength = m_dwMinPasswordAlpha + m_dwMinPasswordNum;
		UpdateData(FALSE);
		m_editMinPasswordLength.SetFocus();
		m_editMinPasswordLength.SetSel(0, -1);
		return FALSE;
	}

	if (m_dwMaxLoginTries > 99)
	{
		sMsg.Format(IDP_MAX_LOGIN_TRIES, 99);
		AfxMessageBox(sMsg, MB_OK);
		m_editMaxLoginTries.SetFocus();
		m_editMaxLoginTries.SetSel(0, -1);
		return FALSE;
	}

	if ((m_dwMinPasswordLength == 0) && m_bUseTPWL)
	{
		AfxMessageBox(IDP_NO_PASSWORD_AND_USE_TPWL, MB_OK);					  
		return FALSE;
	}
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CSecurityPage::SaveChanges()
{
	WK_TRACE_USER(_T("Sicherheits-Einstellungen wurden geändert\n"));
	UpdateData();

	GetProfile().WriteInt(szSecurityLevel,_T("MinPasswordLength"),m_dwMinPasswordLength);
	GetProfile().WriteInt(szSecurityLevel,_T("MinPasswordAlpha"),m_dwMinPasswordAlpha);
	GetProfile().WriteInt(szSecurityLevel,_T("MinPasswordNum"),m_dwMinPasswordNum);
	GetProfile().WriteInt(szSecurityLevel,_T("TimeLimit"),m_dwTimeLimit);
	GetProfile().WriteInt(szSecurityLevel,_T("MaxLoginTries"),m_dwMaxLoginTries);
	GetProfile().WriteInt(szSecurityLevel,_T("UserLockTime"),m_dwUserLockTime);
	GetProfile().WriteInt(szSecurityLevel,_T("UseTPWL"),m_bUseTPWL);

	CSystemTime t;
	t.GetLocalTime();
	GetProfile().WriteString(szSecurityLevel,_T("ModifiedTime"),t.ToString());

	SaveTrivialListToRegistry();
}
/////////////////////////////////////////////////////////////////////////////
void CSecurityPage::CancelChanges()
{
	UpdateData();
	m_dwMinPasswordLength = GetProfile().GetInt(szSecurityLevel,_T("MinPasswordLength"),
												WK_PASSWORD_MIN_LEN);
	m_dwMinPasswordAlpha = GetProfile().GetInt(szSecurityLevel,_T("MinPasswordAlpha"),
												WK_PASSWORD_MIN_ALPHA);
	m_dwMinPasswordNum = GetProfile().GetInt(szSecurityLevel,_T("MinPasswordNum"),
												WK_PASSWORD_MIN_NUM);
	m_dwTimeLimit = GetProfile().GetInt(szSecurityLevel,_T("TimeLimit"),m_dwTimeLimit);
	m_dwMaxLoginTries =	GetProfile().GetInt(szSecurityLevel,_T("MaxLoginTries"),
											m_dwMaxLoginTries);
	m_dwUserLockTime =	GetProfile().GetInt(szSecurityLevel,_T("UserLockTime"),
											m_dwUserLockTime);
	m_bUseTPWL = GetProfile().GetInt(szSecurityLevel,_T("UseTPWL"), m_bUseTPWL);

	LoadTrivialListFromRegistry();

	UpdateData(FALSE);
	ShowHide();
}
/////////////////////////////////////////////////////////////////////////////
void CSecurityPage::ShowHide()
{
	m_EditTPWL.EnableWindow(m_bUseTPWL);
	if (m_bUseTPWL)
		m_EditTPWL.SetBackgroundColor(FALSE, RGB(255, 255, 255));
	else
		m_EditTPWL.SetBackgroundColor(FALSE, RGB(192, 192, 192));
}
/////////////////////////////////////////////////////////////////////////////////
BOOL CSecurityPage::SaveTrivialListToRegistry()
{
	TCHAR szBuffer[1024] = {0};
	CString sLineNr;

	GetProfile().DeleteSection(_T("TPWL"));

	for (int nI = 0; nI < m_EditTPWL.GetLineCount(); nI++)
	{
		int nCnt = m_EditTPWL.GetLine(nI, szBuffer, 1024);
		if (nCnt >= 2)
			szBuffer[nCnt-2] = 0;
		sLineNr.Format(_T("%05d"), nI);
		GetProfile().WriteString(_T("TPWL"), sLineNr, szBuffer);
	}

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////////
BOOL CSecurityPage::LoadTrivialListFromRegistry()
{
	CString sLineNr;
	CString sPassword;
	int nI = 0;
	m_sTrivialPasswordList = _T("");

	BOOL bLoop = TRUE;
	while (bLoop) 
	{
		sLineNr.Format(_T("%05d"), nI++);
		sPassword = GetProfile().GetString(_T("TPWL"), sLineNr, _T(""));
		
		sLineNr.Format(_T("%05d"), nI);
		if (GetProfile().GetString(_T("TPWL"), sLineNr, _T("<<EndeDerListe>>")) != _T("<<EndeDerListe>>"))
		{
			m_sTrivialPasswordList += sPassword;
			m_sTrivialPasswordList += _T("\r\n");
		}
		else
		{
			m_sTrivialPasswordList += sPassword;
			bLoop = FALSE;
		}
	}

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CSecurityPage::DoDataExchange(CDataExchange* pDX)
{
	CSVPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSecurityPage)
	DDX_Control(pDX, IDC_EDIT_MIN_ALPHA, m_editMinPasswordAlpha);
	DDX_Control(pDX, IDC_EDIT_MIN_PASSWORD, m_editMinPasswordLength);
	DDX_Control(pDX, IDC_EDIT_MAX_FAILED, m_editMaxLoginTries);
	DDX_Control(pDX, IDC_EDIT_TPWL, m_EditTPWL);
	DDX_Text(pDX, IDC_EDIT_MIN_PASSWORD,	m_dwMinPasswordLength);
	DDX_Text(pDX, IDC_EDIT_MIN_ALPHA,		m_dwMinPasswordAlpha);
	DDX_Text(pDX, IDC_EDIT_MIN_NUMERIC,		m_dwMinPasswordNum);
	DDX_Text(pDX, IDC_EDIT_TIME_LIMIT,		m_dwTimeLimit);
	DDX_Text(pDX, IDC_EDIT_MAX_FAILED,		m_dwMaxLoginTries);
	DDX_Text(pDX, IDC_EDIT_FAILED_LOCK,		m_dwUserLockTime);
	DDX_Check(pDX, IDC_USE_TPWL, m_bUseTPWL);
	DDX_Text(pDX, IDC_EDIT_TPWL, m_sTrivialPasswordList);
	//}}AFX_DATA_MAP
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CSecurityPage, CSVPage)
	//{{AFX_MSG_MAP(CSecurityPage)
	ON_BN_CLICKED(IDC_USE_TPWL, OnUseTpwl)
	ON_EN_CHANGE(IDC_EDIT_FAILED_LOCK, OnChangeEditFailedLockTime)
	ON_EN_CHANGE(IDC_EDIT_MAX_FAILED, OnChangeEditMaxFailed)
	ON_EN_CHANGE(IDC_EDIT_MIN_ALPHA, OnChangeEditMinAlpha)
	ON_EN_CHANGE(IDC_EDIT_MIN_NUMERIC, OnChangeEditMinNumeric)
	ON_EN_CHANGE(IDC_EDIT_MIN_PASSWORD, OnChangeEditMinPassword)
	ON_EN_CHANGE(IDC_EDIT_TIME_LIMIT, OnChangeEditTimeLimit)
	ON_EN_CHANGE(IDC_EDIT_TPWL, OnChangeEditTpwl)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CSecurityPage message handlers
/////////////////////////////////////////////////////////////////////////////
BOOL CSecurityPage::OnInitDialog() 
{
	CSVPage::OnInitDialog();
	
	// Einen nicht proportionalen Font wählen
	m_fixedFont.CreatePointFont(10*10, GetFixFontFaceName());
	m_EditTPWL.SetFont(&m_fixedFont, TRUE);
 	m_EditTPWL.SetEventMask(ENM_CHANGE);

	CancelChanges();
	
	m_bLoadingTPWL = TRUE;

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
void CSecurityPage::OnChangeEditMinPassword() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CSVPage::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData();
	SetModified();
}
/////////////////////////////////////////////////////////////////////////////
void CSecurityPage::OnChangeEditMinAlpha() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CSVPage::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData();
	SetModified();
}
/////////////////////////////////////////////////////////////////////////////
void CSecurityPage::OnChangeEditMinNumeric() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CSVPage::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData();
	SetModified();
}
/////////////////////////////////////////////////////////////////////////////
void CSecurityPage::OnChangeEditTimeLimit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CSVPage::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData();
	SetModified();
}
/////////////////////////////////////////////////////////////////////////////
void CSecurityPage::OnChangeEditMaxFailed() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CSVPage::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData();
	SetModified();
}
/////////////////////////////////////////////////////////////////////////////
void CSecurityPage::OnChangeEditFailedLockTime() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CSVPage::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData();
	SetModified();
}
/////////////////////////////////////////////////////////////////////////////
void CSecurityPage::OnChangeEditTpwl() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CSVPage::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData();
	SetModified();
}
/////////////////////////////////////////////////////////////////////////////
void CSecurityPage::OnUseTpwl() 
{
	UpdateData();
	SetModified();
	ShowHide();
}
