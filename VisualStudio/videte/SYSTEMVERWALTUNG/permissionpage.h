// PermissionPage.h : header file
//
#include "StatusList.h"
#include "SVPage.h"

class CPermission;
class CPermissionArray;
class CUserArray;
class COutput;
class COutputList;
class CArchivInfo;
class CArchivInfoArray;
/////////////////////////////////////////////////////////////////////////////
// CPermissionPage dialog

class CPermissionPage : public CSVPage
{
	DECLARE_DYNAMIC(CPermissionPage)

// Construction
public:
	CPermissionPage(CSVView* pParent);
	~CPermissionPage();

// Dialog Data
	//{{AFX_DATA(CPermissionPage)
	enum { IDD = IDD_PERMISSIONLIST };
	CButton	m_btnPTZConfig;
	CButton	m_btnAlarmOffSpan;
	CButton	m_btnImageCompare;
	CButton	m_btnShutDown;
	CButton	m_btnSettings;
	CButton	m_btnSecurity;
	CButton	m_btnSdiConfig;
	CButton	m_btnLogView;
	CButton	m_btnDateTime;
	CButton	m_btnDataExport;
	CButton	m_btnBackup;
	CButton	m_btnArchiv;
	CButton	m_btnPermission;
	CButton	m_btnHost;
	CButton	m_btnUser;
	CButton	m_btnLink;
	CButton	m_btnProcess;
	CButton	m_btnTimer;
	CButton	m_btnOutput;
	CButton	m_btnInput;
	CButton	m_btnHardware;
	CButton	m_btnPassword;
	CEdit	m_editName;
	CComboBox	m_cbPriority;
	CComboBox	m_cbTimer;
	CStatusList	m_Archivs;
	CStatusList	m_Outputs;
	CListCtrl	m_Permissions;
	CString	m_Name;
	//}}AFX_DATA


public:
	virtual BOOL CanNew();
	virtual BOOL CanDelete();
	virtual BOOL StretchElements();
	virtual void OnNew();
	virtual void OnDelete();

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPermissionPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPermissionPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnItemchangedListPermission(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeName();
	afx_msg void OnPassword();
	afx_msg void OnCheck();
	afx_msg void OnSelchangeComboPriority();
	afx_msg void OnSelchangeComboTimer();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	virtual void SaveChanges();
	virtual void CancelChanges();
	virtual void Initialize();
	virtual BOOL IsDataValid();
	virtual void OnItemChangedStatusList(CStatusList* pStatusList, int nListItem);
	virtual void OnStatusChangedStatusList(int nDlgItemID, int nListItem);

	// initialization
protected:
	void FillPermissions();
	void FillOutputs();
	void FillArchivs();
	void FillTimer();

	void ClearPermissions();

	int  InsertPermission(CPermission* pPermission, BOOL bSelectIt = FALSE);
	void InsertOutput(const COutput* pOutput);
	void InsertMedia(const CMedia* pMedia);
	void InsertArchivInfo(CArchivInfo* pArchivInfo);

	void OutputsToList(CPermission* pPermission);
	void ListToOutputs(CPermission* pPermission);

	void ArchivsToList(CPermission* pPermission);
	void ListToArchivs(CPermission* pPermission);

	void FlagsToCheckBox(CPermission* pPermission);
	void CheckBoxToFlags(CPermission* pPermission);

	void EnableExceptNew();
	void SelectPermission(int i, BOOL bSetSel = TRUE);

protected:
	//data
	CPermissionArray*	m_pPermissionArray;
	CUserArray*			m_pUserArray;
	COutputList*		m_pOutputList;
	CMediaList*			m_pMediaList;
	CArchivInfoArray*	m_pArchivInfoArray;
	CSecTimerArray*		m_pTimers;
	CPermission*		m_pSelectedPermission;
	int					m_iSelectedItem;
	CString				m_sPassword;
	DWORD				m_dwFlags;
	UINT				m_uStatusArchive;

	BOOL				m_bEnableSaveFlag;
	BOOL				m_bEnableAudio;
	BOOL				m_bEnableTimer;
};
