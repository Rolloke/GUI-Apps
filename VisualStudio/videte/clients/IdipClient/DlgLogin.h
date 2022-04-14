#pragma once
#include "afxwin.h"

class CMainFrame;

// CDlgLogin dialog
class CDlgLogin : public CSkinDialog
{
	DECLARE_DYNAMIC(CDlgLogin)

public:
	CDlgLogin(CWnd* pParent,
			  DWORD dwPermissionFlags =0,
			  BOOL bDoubleLogin = FALSE,
			  BOOL bCenter = TRUE);

	// attributes
public:
	CUser			GetUser() const;
	CPermission		GetPermission() const;
	inline CWnd*	GetMyOwnAndRealParent() const;

public:
	virtual ~CDlgLogin();
protected:
	virtual BOOL StretchElements();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();

	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg void OnNcDestroy();
	afx_msg LRESULT OnUserLoginAlarm(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

public:
	void	SetWindowPosition();

private:
	CUser*  CheckUserLogin();
	LRESULT LoginFailed();
	LRESULT PasswordIsOutOfTime();
	LRESULT	UserIsLocked(CUser* pUser);
	BOOL	ChangePassword(CUser* pUser);
	BOOL	IsTempPassword();
	void	LoginOK();

// Dialog Data
private:
	enum eID { IDD = IDD_LOGIN };
	CWnd*			m_pMyOwnAndRealParent;
	CDlgLogin*		m_pSecondLogin;
	CUser*			m_pUser;
	CPermission*	m_pPermission;
	int				m_nMinPasswordLength;	// Mindestl‰nge des Paﬂwortes
	int				m_nMinPasswordNum;		// Mindestanzahl der Ziffern im Paﬂwort
	int				m_nMinPasswordAlpha;	// Mindestanzahl der Buchstaben im Paﬂwort
	BOOL			m_bSecondLogin;			// Legt fest welches LoginIcon erscheint.
	DWORD			m_dwPermissionFlags;	// Notwendige Permission
	BOOL			m_bCenter;
	CString			m_sUser;
	CString			m_sPassword;
	CSkinStatic		m_txtUser;
	CSkinEdit		m_editUser;
	CSkinStatic		m_txtPassword;
	CSkinEdit		m_editPassword;
	CSkinButton		m_btnLogin;
};
/////////////////////////////////////////////////////////////////////////////
inline CWnd* CDlgLogin::GetMyOwnAndRealParent() const
{
	return m_pMyOwnAndRealParent;
}
