/////////////////////////////////////////////////////////////////////////////
// msgbox.h : header file

#ifndef _MSGBOX_H_
#define _MSGBOX_H_


#define MB_NONE	-1

/////////////////////////////////////////////////////////////////////////////
// CMessageBoxDlg dialog

class CMessageBoxDlg : public CDialog
{
	// Construction
	public:
		CMessageBoxDlg(LPCSTR lpszString, int nType = MB_NONE, CWnd* pParent = NULL, BOOL bStatic = FALSE);
			// Parameter :
			//	lpszString	:	enthält die auszugebende Message
			//	nType		:	MB_NONE enthält nur Text, keinen Button
			//					MB_OK enthält OK-Button
			//	pParent		:	ParentWindow
			//	bStatic		:	Objekt wird bei OK-Button nur versteckt, nicht zerstört
		CMessageBoxDlg(int nMSG_ID, int nType = MB_NONE, CWnd* pParent = NULL, BOOL bStatic = FALSE);
			// Parameter :	nMSG_ID enthält die Resource-ID der auszugebenden Message
		~CMessageBoxDlg();

		void Show();
			// Zeigt das Fenster an
		void SetText(CString strMsg);
			// Setzt den Text der Dialogbox

	// Dialog Data
	protected:
		//{{AFX_DATA(CMessageBoxDlg)
		enum { IDD = DLG_MSGBOX_OK };
			// NOTE: the ClassWizard will add data members here
		//}}AFX_DATA
		CWnd*		m_pWndParent;
		CString		m_strMessage;
					// Hier hinein wird die Message aus lpszString kopiert
		BOOL		m_bStatic;
					// TRUE  : Objekt wird bei OK-Button nur versteckt, nicht zerstört
					// FALSE : Objekt wird bei OK-Button zerstört
	
		int			GetTypeRes(int nType);
					// Liefert die Resource-ID in Abhängigkeit vom Messagebox-Typ
	
	// Implementation
	protected:
		virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	
		// Generated message map functions
		//{{AFX_MSG(CMessageBoxDlg)
		virtual void OnOK();
		virtual BOOL OnInitDialog();
	//}}AFX_MSG
		DECLARE_MESSAGE_MAP()
};
#endif // _MSGBOX_H_
/////////////////////////////////////////////////////////////////////////////
// end of msgbox.h
