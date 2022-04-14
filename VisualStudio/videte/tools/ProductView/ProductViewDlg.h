/* GlobalReplace: m_ctlAllow --> m_cbtAllow */
/* GlobalReplace: m_bRunVCUnit --> m_bRunVCSUnit */
/* GlobalReplace: m_ctbRunCDRWriter --> m_cbtRunCDRWriter */
/* GlobalReplace: GAAUnit --> GAUnit */
// ProductViewDlg.h : header file
//

#if !defined(AFX_PRODUCTVIEWDLG_H__3688BAF7_1955_11D1_B8C8_0060977A76F1__INCLUDED_)
#define AFX_PRODUCTVIEWDLG_H__3688BAF7_1955_11D1_B8C8_0060977A76F1__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "ProductView.h"
#include "DialogDongle.h"

/////////////////////////////////////////////////////////////////////////////
// CProductViewDlg dialog
class CProductViewDlg : public CDialog
{

	enum EnumToggleCodecs
	{
		eToggleCodecToQ,
		eToggleCodecToJacob,
		eToggleCodecToSavic,
		eToggleCodecToTasha,
		eToggleCodecUnknown	
	};

// Construction
public:
	CProductViewDlg(CWnd* pParent = NULL);	// standard constructor

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProductViewDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CProductViewDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnDoSave();
	afx_msg void OnSelchangeProductName();
	afx_msg void OnSelchangeOemCode();
	afx_msg void OnSelchangeNrOfCameras();
	afx_msg void OnRunJacob1();
	afx_msg void OnCrypt();
//	afx_msg void OnRestrictedTo8CamerasPerBoard();
	afx_msg void OnRunSavic1();
//	afx_msg void OnRunSavic2();
//	afx_msg void OnRunSavic3();
//	afx_msg void OnRunSavic4();
	afx_msg void OnRunTasha1();
//	afx_msg void OnRunTasha2();
//	afx_msg void OnRunTasha3();
//	afx_msg void OnRunTasha4();
//	afx_msg void OnRunIcpconUnit();
//	afx_msg void OnRunUsbCamera();
//	afx_msg void OnRunAudioUnit();
	afx_msg void OnRadioSerialOrHddID();
	//}}AFX_MSG
	afx_msg long DoConfirmSelfcheck(WPARAM wParam, LPARAM lParam);
	
	DECLARE_MESSAGE_MAP()
private:
	void FillAllButtonsArray();
	void SetFromDongle();
	BOOL SetCurSelByData(CComboBox &combo, DWORD dwData);
	void InitConstraints();
	void UpdateConstraints();
	void UpdateConstraintsIdip();
	void UpdateFunctionalityConstraints();
	
	void SomeCameraUnits();
	void SomeSavic();
	void SomeIP();
	
	void NoJacob();
	void NoSavic();
	void NoTasha();
	void NoQ();
	void NoIP();
	
	void NoMicoNorJacob();
	void NoCameraUnits();
	
	BOOL PreCheckSave();
	void UpdateLabelsForOEM(WORD oemCode);
	void UpdateVisibilityForOEM(WORD wOEM);
	void InitProductNames();
	void UpdateProductNames();
	void HideAllWindows(BOOL bHide);
	void ResetAllFlags();
	void SetValidProduct(InternalProductCode ipc);

	inline BOOL IsNewIdipGeneration(InternalProductCode ipc) const;
	inline InternalProductCode GetCurrentProduct() const;
	inline WORD GetCurrentOEMCode() const;
	inline void UpdateDialogFromMembers();
	inline void UpdateMembersFromDialog();

// Dialog Data
public:
	CDialogDongle m_dongle;
private:
	//{{AFX_DATA(CProductViewDlg)
	enum { IDD = IDD_PRODUCTVIEW_DIALOG };
	CEdit	m_editSerial;
	CButton	m_btnSerial;
	CButton	m_btnHDD;
	CButton	m_cbtRunAudioUnit;
	CButton	m_cbtRunUSBCameraUnit;
	CButton	m_cbtRunICPCONUnit;
	CButton	m_cbtRunSaVicUnit4;
	CButton	m_cbtRunSaVicUnit3;
	CButton	m_cbtRunSaVicUnit2;
	CButton	m_cbtRunSaVicUnit1;
	CButton	m_cbtRunTashaUnit4;
	CButton	m_cbtRunTashaUnit3;
	CButton	m_cbtRunTashaUnit2;
	CButton	m_cbtRunTashaUnit1;
	CStatic	m_ctlVersion;
	CButton	m_cbtRestrictedTo8CamerasPerBoard;
	CStatic	m_staticHDD2;
	CEdit	m_editHDD2;
	CEdit	m_editHDD1;
	CButton	m_buttonCrypt;
	CButton	m_cbtRunHTTP;
	CButton	m_cbtRunFAXUnit;
	CButton	m_cbtRunEMailUnit;
	CButton	m_cbtAllowMotionDetection;
	CButton	m_cbtRunGemosUnit;
	CButton	m_cbtRunJaCobUnit4;
	CButton	m_cbtRunJaCobUnit3;
	CButton	m_cbtRunJaCobUnit2;
	CButton	m_cbtRunJaCobUnit1;
	CButton	m_cbtRunSMSUnit;
	CButton	m_cbtAllowSoftDecodePT;
	CButton	m_cbtAllowSDIAccessControl;
	CButton	m_cbtAllowSDIIndustrialEquipment;
	CButton	m_cbtAllowSDICashSystems;
	CButton	m_cbtAllowSDIParkmanagment;
	CButton	m_cbtRunPTUnit2;
	CButton	m_cbtRunISDNUnit2;
	CButton	m_cbtAllowRouting;
	CStatic	m_ctlSerialLabel;
	CStatic	m_ctlSerial;
	CButton	m_cbtRunCDRWriter;
	CButton	m_cbtAllowInternalAccess;
	CButton	m_cbtRunSDIConfig;
	CComboBox	m_cbOEMCode;
	CStatic	m_ctFullVersion;
	CButton	m_cbtAllowLocalProcesses;
	CButton	m_cbtRunSecAnalyzer;
	CButton	m_cbtRunSimUnit;
	CButton	m_cbtRunSDIUnit;
	CButton	m_cbtRunGAUnit;
	CButton	m_cbtRunCommUnit;
	CButton	m_cbtRunAKUUnit;
	CButton	m_cbtRunServer;
	CStatic	m_ctFullProductName;
	CButton	m_cbtAllowMultiMonitor;
	CButton	m_cbtAllowTimers;
	CButton	m_cbtAllowHTMLmaps;
	CButton	m_cbtRunVCSUnit;
	CButton	m_cbtRunSocketUnit;
	CButton	m_cbtRunPTUnit;
	CButton	m_cbtRunTOBSUnit;
	CButton	m_cbtRunIPCameraUnit;
	CButton	m_cbtRunISDNUnit;
	CButton	m_cbtRunQUnit;
	CButton	m_buttonSave;
	CComboBox	m_cbProduct;
	CComboBox   m_cbNrOfCameras;
	CComboBox   m_cbNrOfIPCameras;
	CString	m_sProductName;
	CSpinButtonCtrl m_spinNoOfHosts;
	BOOL	m_bRunAKUUnit;
	BOOL	m_bRunCDRWriter;
	BOOL	m_bRunQUnit;
	BOOL	m_bRunCommUnit;
	BOOL	m_bRunGAUnit;
	BOOL	m_bRunISDNUnit;
	BOOL	m_bRunIPCameraUnit;
	BOOL	m_bRunPTUnit;
	BOOL	m_bRunTOBSUnit;
	BOOL	m_bRunSDIUnit;
	BOOL	m_bRunServer;
	BOOL	m_bRunSimUnit;
	BOOL	m_bRunSocketUnit;
	BOOL	m_bRunVCSUnit;
	BOOL	m_bRunExplorer;
	CString	m_sFullProductName;
	BOOL	m_bRunSecAnalyzer;
	BOOL	m_bAllowLocalProcesses;
	CString	m_sFullVersionText;
	BOOL	m_bRunSDIConfig;
	BOOL	m_bAllowInternalAccess;
	CString	m_sSerial;
	BOOL	m_bAllowRouting;
	BOOL	m_bRunISDNUnit2;
	BOOL	m_bRunPTUnit2;
	BOOL	m_bAllowSDICashSystems;
	BOOL	m_bAllowSDIIndustrialEquipment;
	BOOL	m_bAllowSDIAccessControl;
	BOOL	m_bAllowSDIParkmanagment;
	BOOL	m_bAllowSoftDecodePT;
	BOOL	m_bRunSMSUnit;
	BOOL	m_bRunJaCobUnit1;
	BOOL	m_bRunJaCobUnit2;
	BOOL	m_bRunJaCobUnit3;
	BOOL	m_bRunJaCobUnit4;
	BOOL	m_bRunGemosUnit;
	BOOL	m_bAllowMotionDetection;
	BOOL	m_bAllowMultiMonitor;
	BOOL	m_bAllowHTMLmaps;
	BOOL	m_bAllowTimers;
	BOOL	m_bRunEMailUnit;
	BOOL	m_bRunFAXUnit;
	BOOL	m_bRunHTTP;
	BOOL	m_bRestrictedTo8CamerasPerBoard;
	BOOL	m_bRunSaVicUnit1;
	BOOL	m_bRunSaVicUnit2;
	BOOL	m_bRunSaVicUnit3;
	BOOL	m_bRunSaVicUnit4;
	BOOL	m_bRunTashaUnit1;
	BOOL	m_bRunTashaUnit2;
	BOOL	m_bRunTashaUnit3;
	BOOL	m_bRunTashaUnit4;
	BOOL	m_bRunAudioUnit;
	BOOL	m_bRunICPCONUnit;
	BOOL	m_bRunUSBCameraUnit;
	int		m_iSerialOrHddID;
	
	int		m_iNoOfHosts;
	int		m_iNrOfCameras;
	int		m_iNrOfIPCameras;

	//}}AFX_DATA
	HICON				m_hIcon;
	CPtrArray			m_allButtons;
	EnumToggleCodecs	m_eToggleCodecs;

	// friend class for protected member access
	friend CDialogDongle;
public:
	afx_msg void OnDeltaposSpinNoOfHosts(NMHDR *pNMHDR, LRESULT *pResult);
};
/////////////////////////////////////////////////////////////////////////////
inline BOOL CProductViewDlg::IsNewIdipGeneration(InternalProductCode ipc) const
{
	BOOL bRet = FALSE;
	switch (ipc)
	{
		case IPC_IDIP_4:
		case IPC_IDIP_8:
		case IPC_IDIP_16:
		case IPC_IDIP_24:
		case IPC_IDIP_32:
		case IPC_IDIP_PROJECT:
		case IPC_IDIP_XL_4:
		case IPC_IDIP_XL_8:
		case IPC_IDIP_XL_16:
		case IPC_IDIP_XL_24:
		case IPC_IDIP_XL_32:
		case IPC_IDIP_ML:
		case IPC_IDIP_IP:
			bRet = TRUE;
			break;
	}
	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
inline InternalProductCode CProductViewDlg::GetCurrentProduct() const
{
	// each record has the IPC attached to it, so order is not important
	return (InternalProductCode) m_cbProduct.GetItemData( m_cbProduct.GetCurSel() );
}
/////////////////////////////////////////////////////////////////////////////
inline WORD CProductViewDlg::GetCurrentOEMCode() const
{
	// each record has the OEMCode attached to it, so order is not important
	return (WORD)(m_cbOEMCode.GetItemData( m_cbOEMCode.GetCurSel() ));
}
/////////////////////////////////////////////////////////////////////////////
inline void CProductViewDlg::UpdateDialogFromMembers()
{
	UpdateData(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
inline void CProductViewDlg::UpdateMembersFromDialog()
{
	UpdateData(TRUE);
}
/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.
/////////////////////////////////////////////////////////////////////////////
#endif // !defined(AFX_PRODUCTVIEWDLG_H__3688BAF7_1955_11D1_B8C8_0060977A76F1__INCLUDED_)
