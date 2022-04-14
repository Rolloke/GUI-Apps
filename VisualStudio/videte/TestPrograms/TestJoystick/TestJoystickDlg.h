// TestJoystickDlg.h : Headerdatei
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "TargetCrossControl.h"
#include "POVControl.h"

class CJoyStickDX;
struct DIJOYSTATE;
struct DIJOYSTATE2;
// CTestJoystickDlg Dialogfeld
class CTestJoystickDlg : public CDialog
{
// Konstruktion
public:
	CTestJoystickDlg(CWnd* pParent = NULL);	// Standardkonstruktor
	virtual ~CTestJoystickDlg();	// Standardkonstruktor

// Dialogfelddaten
	enum { IDD = IDD_TESTJOYSTICK_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV-Unterstützung

	void OnCkBtn(UINT);
// Implementierung
protected:
	HICON m_hIcon;
	CJoyStickDX *m_pJoyStick;
	DIJOYSTATE	*m_pJoyState;
	DIJOYSTATE2	*m_pJoyState2;

	CSliderCtrl m_SliderZ;
	
	CSliderCtrl m_SliderRZ;

	CSliderCtrl m_Slider1;
	CSliderCtrl m_Slider2;
	int			m_nButtonMap[8];
	BOOL		m_bLearnMode;
	CEdit m_Output;
	CButton m_btnChooseJoystick;
	CTargetCrossControl m_ctrlTarget;
	CTargetCrossControl m_ctrlTargetR;
	CPOVControl	m_ctrlPOV;

	void RegisterHotKey(UINT uID);
	void _cdecl OutputDebug(LPCTSTR lpszFormat, ...);
	// Generierte Funktionen für die Meldungstabellen
	virtual BOOL OnInitDialog();

	virtual void OnCancel();

	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnUser(WPARAM wParam, LPARAM lParam);
	afx_msg void OnDestroy();
	afx_msg void OnNcDestroy();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg LRESULT OnHotKey(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedBtnReset();
	afx_msg void OnBnClickedBtnChooseJoystick();
	DECLARE_MESSAGE_MAP()
public:
};
