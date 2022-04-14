// TestJoystickDlg.h : Headerdatei
//

#pragma once

class CJoyStickDX;
// CTestJoystickDlg Dialogfeld
class CTestJoystickDlg : public CDialog
{
// Konstruktion
public:
	CTestJoystickDlg(CWnd* pParent = NULL);	// Standardkonstruktor

// Dialogfelddaten
	enum { IDD = IDD_TESTJOYSTICK_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV-Unterstützung


// Implementierung
protected:
	HICON m_hIcon;
	CJoyStickDX *m_pJoyStick;
	// Generierte Funktionen für die Meldungstabellen
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
};
