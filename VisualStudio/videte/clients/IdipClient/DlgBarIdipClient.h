// DlgBaridipClient.h: interface for the CDlgBarIdipClient class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DLGBARIDIPCLIENT_H__)
#define AFX_DLGBARIDIPCLIENT_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
#pragma once

#include "ViewDlg.h"
#include "ButtonUpDown.h"

// CDlgBarIdipClient dialog

class CWndLive;
class CIPCInputIdipClient; 
class CIPCOutputIdipClient; 


class CDlgBarIdipClient : public CDlgInView
{
	DECLARE_DYNAMIC(CDlgBarIdipClient)

public:
	CDlgBarIdipClient(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgBarIdipClient();
	void SetPTZWindow(CWndLive*pWndLive);

protected:
	CWnd *	GetDlgItem(CRuntimeClass*pRC, CWnd*pWndPrevious);
	CSkinButton& GetSkinButton(int);
	CImageList * CreateImageList(UINT nBmpId, int nWidth);
	void PostCommandID(UINT nID);
	BOOL OnNum(UINT nID, UINT nRepCnt, UINT nFlags);
	BOOL OnButtonUp(UINT nID, UINT nRepCnt, UINT nFlags);
	BOOL OnButtonDown(UINT nID, UINT nRepCnt, UINT nFlags);

	//	PTZ-Controls
	void EnablePTZControls(DWORD dwFunctions);
	void SetPTZFunctions(DWORD dwFunctions, DWORD dwFunctionsEx);
	void SetPTZType(CameraControlType ccType);
	void TogglePresetMode();
	int  GetIndexFromID(UINT nID);
	CameraControlCmd GetStopCmd(CameraControlCmd nCmd);
public:
	BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	BOOL OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	BOOL OnKeyboardChanged(CKBInput*pActive);
	void OnISDNBchannels(WORD wChannels, LPCTSTR sText);

// Dialog Data
	//{{AFX_DATA(CDlgBarIdipClient)
	enum  { IDD = IDD_IDIP_CLIENT_DLG_BAR };

	CSkinSlider m_SpeedCtrl;
	CButtonUpDown	m_IrisOutButton;
	CButtonUpDown	m_IrisInButton;
	CButtonUpDown	m_ZoomOutButton;
	CButtonUpDown	m_ZoomInButton;
	CButtonUpDown	m_FocusOutButton;
	CButtonUpDown	m_FocusInButton;
	CButtonUpDown	m_UpButton;
	CButtonUpDown	m_RightButton;
	CButtonUpDown	m_LeftButton;
	CButtonUpDown	m_DownButton;
	BOOL	m_bFocusAuto;
	BOOL	m_bIrisAuto;
	BOOL	m_bDome;
	//}}AFX_DATA
//	PTZ-Controls
	CWndLive*				m_pWndLive;
	CIPCInputIdipClient*	m_pInput; 
	CIPCOutputIdipClient*	m_pOutput; 

	CSecID					m_commID;
	CSecID					m_camID;
	CameraControlType		m_ccType;
	DWORD					m_dwPTZFunctions;
	DWORD					m_dwPTZFunctionsEx;
	BOOL					m_bPreset;
	DWORD					m_dwSpeed;
	CImageList				m_imgPTZ;

	static const UINT				gm_nPTZF_POS[];
	static const UINT				gm_nIndexID[];
	static const CameraControlCmd	gm_nCamCtrlCmd[];
	static CSkinButton				gm_DummySkinButton;
protected:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgBarIdipClient)
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	virtual BOOL StretchElements();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL GetToolTip(UINT nID, CString&sText);
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CDlgBarIdipClient)
	afx_msg LRESULT OnUserMsg(WPARAM, LPARAM);
	afx_msg void OnCommandRange(UINT nID);
	afx_msg void OnBnClickedCloseDlgBar();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnCenter();
	afx_msg void OnPreset();
	afx_msg void OnFocusAuto();
	afx_msg void OnIrisAuto();
	afx_msg void OnTurn180();
	afx_msg void OnNum(UINT nID);
	afx_msg void OnButtonUp(UINT nID);
	afx_msg void OnButtonDown(UINT nID);
	afx_msg void OnUpdateCmdPTZ(CCmdUI*pCmdUI);
	afx_msg void OnUpdateButtonDome(CCmdUI*pCmdUI);
	afx_msg void OnUpdateButtonPreset(CCmdUI*pCmdUI);
	afx_msg void OnUpdateKbLang(CCmdUI *pCmdUI);
	afx_msg void OnKbLang();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnUpdateBtnISDN(CCmdUI *pCmdUI);
	afx_msg void OnBtnISDN();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
};
#endif