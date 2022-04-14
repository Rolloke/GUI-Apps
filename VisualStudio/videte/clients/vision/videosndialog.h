/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: videosndialog.h $
// ARCHIVE:		$Archive: /Project/Clients/Vision/videosndialog.h $Date: 16.08.96 12:49 $
// VERSION:		$Revision: 11 $
// LAST CHANGE:	$Modtime: 15.10.03 11:36 $
// BY AUTHOR:	$Author: Georg.feddern $
// $Nokeywords:$

#ifndef _VideoSNDialog_H
#define _VideoSNDialog_H

class CDisplayWindow;
class CIPCInputVision;
class CIPCOutputVision;

/////////////////////////////////////////////////////////////////////////////
// CUpDownButton window
#define BN_UP          8
#define BN_DOWN        9

#define ON_BN_UP(id, memberFxn) \
	ON_CONTROL(BN_UP, id, memberFxn)
#define ON_BN_DOWN(id, memberFxn) \
	ON_CONTROL(BN_DOWN, id, memberFxn)

class CUpDownButton : public CButton
{
// Construction
public:
	CUpDownButton();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUpDownButton)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CUpDownButton();

	// Generated message map functions
protected:
	//{{AFX_MSG(CUpDownButton)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// CVideoSNDialog dialog

class CVideoSNDialog : public CWK_Dialog
{
// Construction
public:
	CVideoSNDialog(	CIPCInputVision* pInput,CIPCOutputVision* pOutput,
					CameraControlType ccType, DWORD dwPTZFunctions,
					CWnd* pParent = NULL);   // standard constructor
	~CVideoSNDialog();
	DECLARE_DYNAMIC(CVideoSNDialog)

// Dialog Data
	//{{AFX_DATA(CVideoSNDialog)
	enum { IDD = IDD_VIDEO_SN };
	CButton	m_Turn180Button;
	CButton	m_IrisAutoButton;
	CButton	m_FocusAutoButton;
	CSliderCtrl	m_SpeedCtrl;
	CButton	m_PresetButton;
	CButton	m_Pos9Button;
	CButton	m_Pos8Button;
	CButton	m_Pos7Button;
	CButton	m_Pos6Button;
	CButton	m_Pos5Button;
	CButton	m_Pos4Button;
	CButton	m_Pos3Button;
	CButton	m_Pos2Button;
	CButton	m_Pos1Button;
	CButton	m_HomeButton;
	CUpDownButton	m_IrisOutButton;
	CUpDownButton	m_IrisInButton;
	CUpDownButton	m_ZoomOutButton;
	CUpDownButton	m_ZoomInButton;
	CUpDownButton	m_FocusOutButton;
	CUpDownButton	m_FocusInButton;
	CUpDownButton	m_UpButton;
	CUpDownButton	m_RightButton;
	CUpDownButton	m_LeftButton;
	CUpDownButton	m_DownButton;
	BOOL	m_bFocusAuto;
	BOOL	m_bIrisAuto;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVideoSNDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	virtual void OnCancel();
	virtual void OnOK();
	//}}AFX_VIRTUAL

public:
	void SetPTZFunctions(DWORD dwFunctions);
	void SetPTZType(CameraControlType ccType);
	void TogglePresetMode();

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CVideoSNDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnUpUp();
	afx_msg void OnUpDown();
	afx_msg void OnDownUp();
	afx_msg void OnDownDown();
	afx_msg void OnLeftUp();
	afx_msg void OnLeftDown();
	afx_msg void OnRightUp();
	afx_msg void OnRightDown();
	afx_msg void OnZoominUp();
	afx_msg void OnZoominDown();
	afx_msg void OnZoomoutUp();
	afx_msg void OnZoomoutDown();
	afx_msg void OnFocusinUp();
	afx_msg void OnFocusinDown();
	afx_msg void OnFocusoutUp();
	afx_msg void OnFocusoutDown();
	afx_msg void OnIrisinUp();
	afx_msg void OnIrisinDown();
	afx_msg void OnIrisoutUp();
	afx_msg void OnIrisoutDown();
	afx_msg void OnCenter();
	afx_msg void OnPos1();
	afx_msg void OnPos2();
	afx_msg void OnPos3();
	afx_msg void OnPos4();
	afx_msg void OnPos5();
	afx_msg void OnPos6();
	afx_msg void OnPos7();
	afx_msg void OnPos8();
	afx_msg void OnPos9();
	afx_msg void OnPreset();
	afx_msg void OnFocusAuto();
	afx_msg void OnIrisAuto();
	afx_msg void OnTurn180();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void EnablePTZControls(DWORD dwFunctions);

private:
	CDisplayWindow* m_pDisplayWindow;
	CIPCInputVision* m_pInput; 
	CIPCOutputVision* m_pOutput; 

	CSecID m_commID;
	CSecID m_camID;

	CameraControlType	m_ccType;
	DWORD	m_dwPTZFunctions;
	BOOL	m_bPreset;
	DWORD m_dwSpeed;
	friend class CVisionApp;
};
/////////////////////////////////////////////////////////////////////////////
#endif
