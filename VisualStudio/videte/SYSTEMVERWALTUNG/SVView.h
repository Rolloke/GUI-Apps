// SVView.h : interface of the CSVView class
//
#ifndef	_SV_VIEW_H
#define _SV_VIEW_H
/////////////////////////////////////////////////////////////////////////////
class CSVTree;
class CSVPage;
class CSVPropertySheet;
class CSVDoc;
class CPropPageVariable;

#define TREEITEM_UP _T(" <<-- ")

typedef CMap<void*,void*,DWORD,DWORD> CMapPtrToDWORD;


class CSVView : public CView
{
protected: // create from serialization only
	CSVView();
	DECLARE_DYNCREATE(CSVView)

	struct sRoomSurveillance
	{
		sRoomSurveillance();
		BOOL			m_bSetNames;
		BOOL			m_bUVVK;
		BOOL			m_bMD;
		DWORD			m_dwBoardActive;
		DWORD			m_dwBoardMD;
		int				m_nInitialPageID;
		CSecID			m_ProcessID;
		CSecID			m_ProcessIDprering;
		CStringArray	m_OutputSM;
		CStringArray	m_InputSM;
		CMapPtrToPtr	m_Outputs;
		CMapPtrToDWORD	m_Archives;
	};

// Attributes
public:
	CSVDoc* GetDocument();
	BOOL	CanCloseFrame(BOOL bInit = TRUE);
  	CSVPage* GetSVPage();
  	CSVTree* GetSVTree();
	CSVPropertySheet * GetSVWizard();

// Operations
public:
	void InitTree();
	void ClearHTREEITEMs();
	void Clear();

	void TreeSelchanged(HTREEITEM hNewSelItem);

	void OnInputChanged();
	void OnCameraChanged();
	void OnRelaisChanged();
	void OnHardwareChanged();

	void AddInitApp(const CString& sAppname);

	HTREEITEM FindItem(HTREEITEM, DWORD, LPCTSTR);
	void EmptyWizardPointer();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSVView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL
public:
	virtual void OnIdleSVView();

// Implementation
public:
	virtual ~CSVView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	void Resize();

	void InitSystem();
	void InitInput();
	void InitOutput();
	void InitAudio(HTREEITEM hInsertAfter=TVI_LAST);
	void InitHost();
	void InitSecurity();
	void InitTimer();
	void InitProcesses();
	void InitNotification();
	void InitDatabase();
	void InitSDI();

private:
	static BOOL WizardHWBActivateSetActive(CPropPageVariable*, LPARAM);
	static BOOL WizardHWBActivateNext(CPropPageVariable*, LPARAM);

	static BOOL WizardHWBActivateMDSetActive(CPropPageVariable*, LPARAM);
	static BOOL WizardHWBActivateMDNext(CPropPageVariable*, LPARAM);

	static BOOL WizardActivateCamerasInit(CPropPageVariable*, LPARAM);
	static BOOL WizardActivateCamerasSetActive(CPropPageVariable*, LPARAM);
	static BOOL WizardActivateCamerasNext(CPropPageVariable*, LPARAM);

	static BOOL WizardActivateMDInputInit(CPropPageVariable*, LPARAM);
	static BOOL WizardActivateMDInputSetActive(CPropPageVariable*, LPARAM);

	static BOOL WizardInitDrivesSetActive(CPropPageVariable*, LPARAM);
	
	static BOOL WizardInitMDArchivesSetActive(CPropPageVariable*, LPARAM);

	static BOOL WizardActivateSaveProcessSetActive(CPropPageVariable*, LPARAM);
	static BOOL WizardActivateSaveProcessNext(CPropPageVariable*, LPARAM);
	
	static BOOL WizardActivateMDActivateSetActive(CPropPageVariable*, LPARAM);

// Generated message map functions
protected:
	//{{AFX_MSG(CSVView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnFileSave();
	afx_msg void OnUpdateFileSave(CCmdUI* pCmdUI);
	afx_msg void OnEditNew();
	afx_msg void OnUpdateEditNew(CCmdUI* pCmdUI);
	afx_msg void OnEditCancel();
	afx_msg void OnUpdateEditCancel(CCmdUI* pCmdUI);
	afx_msg void OnEditDelete();
	afx_msg void OnUpdateEditDelete(CCmdUI* pCmdUI);
	afx_msg void OnEditServerinit();
	afx_msg void OnUpdateEditServerinit(CCmdUI* pCmdUI);
	afx_msg void OnViewTooltips();
	afx_msg void OnUpdateViewTooltips(CCmdUI* pCmdUI);
	afx_msg void OnWizFirst();
	afx_msg void OnUpdateWizFirst(CCmdUI* pCmdUI);
	afx_msg void OnWizRoomSurveillance(UINT nID);
	afx_msg void OnUpdateWizRoomSurveillanceMd(CCmdUI* pCmdUI);
	afx_msg void OnUpdateWizPermanentRecording(CCmdUI* pCmdUI);
	afx_msg void OnUpdateWizUvvkRecording(CCmdUI* pCmdUI);
	afx_msg LRESULT OnSelectPage(WPARAM,LPARAM);
	afx_msg LRESULT OnSetWizardSize(WPARAM,LPARAM);
	afx_msg LRESULT OnUserMessage(WPARAM, LPARAM);
	afx_msg void OnDestroy();
	afx_msg void OnWizDesignaPM100Recording();
	afx_msg void OnUpdateWizDesignaPM100Recording(CCmdUI *pCmdUI);
	afx_msg void OnWizDesignaPMAbacusRecording();
	afx_msg void OnUpdateWizDesignaPMAbacusRecording(CCmdUI *pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	BOOL				m_bServerInit;
private:
	BOOL				m_bInitApps;
	BOOL				m_bShowToolTips;

	CSVTree*			m_pSVTree;
	CSVPage*			m_pSVPage;
	CSVPropertySheet*	m_pSVWizard;
	CRect				m_PageRect;
	BOOL				m_bResizing;
	BOOL				m_bClearing;
	long				m_lTreeWidth;
	long				m_lWizardHeight;

	HTREEITEM   m_SelectedItem;
	HTREEITEM   m_DeletedItem;
	sRoomSurveillance*m_pRoomSurveillanceMd;

	// my tree items
	HTREEITEM	m_hSystemPage;
		HTREEITEM	m_hHardwarePage;
			HTREEITEM	m_hQUnitPage;
			HTREEITEM	m_hIPCameraPage;
			HTREEITEM	m_hJaCobPage1;
			HTREEITEM	m_hJaCobPage2;
			HTREEITEM	m_hJaCobPage3;
			HTREEITEM	m_hJaCobPage4;
			HTREEITEM	m_hSaVicPage1;
			HTREEITEM	m_hSaVicPage2;
			HTREEITEM	m_hSaVicPage3;
			HTREEITEM	m_hSaVicPage4;
			HTREEITEM	m_hTashaPage1;
			HTREEITEM	m_hTashaPage2;
			HTREEITEM	m_hTashaPage3;
			HTREEITEM	m_hTashaPage4;
			HTREEITEM	m_hAudioPage[SM_NO_OF_AUDIO_UNITS];
			HTREEITEM	m_hICPCONPage;
			HTREEITEM	m_hSTMPage;
			HTREEITEM	m_hYUTAPage;
			HTREEITEM	m_hJoystickPage;
			HTREEITEM	m_hQuadPage;
		HTREEITEM	m_hLinkPage;
			HTREEITEM	m_hISDNPage1;
			HTREEITEM	m_hISDNPage2;
			HTREEITEM	m_hPTUnitPage1;
			HTREEITEM	m_hPTUnitPage2;
			HTREEITEM	m_hTOBSUnitPage;
			HTREEITEM	m_hSocketPage;
			HTREEITEM	m_hSMSPage;
			HTREEITEM	m_hEMailPage;
		HTREEITEM	m_hSoftwarePage;
			HTREEITEM	m_hSimPage;
			HTREEITEM	m_hRS232Page;
			HTREEITEM	m_hMESZPage;
			HTREEITEM	m_hAutoLogoutPage;
			HTREEITEM	m_hNTLoginPage;
			HTREEITEM	m_hGemosPage;
		HTREEITEM	m_hMultiMonitorPage;
		HTREEITEM	m_hLanguagePage;
		HTREEITEM	m_hMaintenancePage;
		HTREEITEM	m_hMiscellaneousPage;
	HTREEITEM	m_hDatabase;
		HTREEITEM m_hDrives;
		HTREEITEM m_hArchivs;
		HTREEITEM m_hDatabaseFields;
	HTREEITEM	m_hSDIPage;
	HTREEITEM	m_hInputPage;
	HTREEITEM	m_hCameraPage;
	HTREEITEM	m_hRelaisPage;
	HTREEITEM	m_hAudioChannel;
		HTREEITEM	m_hAudioPageIn;
		HTREEITEM	m_hAudioPageOut;
	HTREEITEM	m_hSecurityPage;
		HTREEITEM	m_hPermissionPage;
		HTREEITEM	m_hUserPage;
	HTREEITEM	m_hTimerPage;
	HTREEITEM	m_hHostPage;
	HTREEITEM	m_hNotificationPage;
	HTREEITEM	m_hProcesses;
		HTREEITEM	m_hCallProcessPage;
		HTREEITEM	m_hSaveProcessPage;
		HTREEITEM	m_hBackupProcessPage;
		HTREEITEM	m_hActualImagePage;
		HTREEITEM	m_hImageCallPage;
		HTREEITEM	m_hCheckCallPage;
		HTREEITEM	m_hGuardTourPage;
		HTREEITEM	m_hVideoOutPage;
		HTREEITEM   m_hUploadPage;
	HTREEITEM	m_hActivationPage;
};

#ifndef _DEBUG  // debug version in SVView.cpp
inline CSVDoc* CSVView::GetDocument()
{
   return (CSVDoc*)m_pDocument; 
}

inline CSVPage* CSVView::GetSVPage()
{
   return m_pSVPage;
}

inline CSVTree* CSVView::GetSVTree()
{
   return m_pSVTree;
}
#endif

/////////////////////////////////////////////////////////////////////////////
#endif
