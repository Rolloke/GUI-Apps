/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: activationpage.h $
// ARCHIVE:		$Archive: /Project/SystemVerwaltung/activationpage.h $
// CHECKIN:		$Date: 14.12.05 21:41 $
// VERSION:		$Revision: 24 $
// LAST CHANGE:	$Modtime: 14.12.05 21:34 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#ifndef ActivationPage_H
#define ActivationPage_H

#include "StatusList.h"
#include "SVPage.h"

class CSecTimerArray;

/////////////////////////////////////////////////////////////////////////////
// CActivationPage dialog
class CActivationPage : public CSVPage
{
	DECLARE_DYNAMIC(CActivationPage)

// Construction
public:
	CActivationPage(CSVView* pParent);
	~CActivationPage();

	//
	inline COutputList*	GetOutputList() const { return m_pOutputList; }
	inline CProcessList*	GetProcessList() const {return m_pProcessList;}
	inline CSecTimerArray*	GetTimerList() const { return m_pTimerList;}

public:
	virtual void OnNew();
	virtual void OnDelete();
	virtual BOOL CanNew();
	virtual BOOL CanDelete();
	virtual BOOL StretchElements();

// Overrides
protected:
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CActivationPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	virtual BOOL IsDataValid();
	virtual void OnItemChangedStatusList(CStatusList* pStatusList, int nListItem);

// Implementation
	void OnItemChangedTimerList(int nListItem);
	void OnItemChangedOutputList(int nListItem);
	void OnItemChangedProcessList(int nListItem);
	void OnItemChangedArchivList(int nListItem);
	void OnItemChangedNotificationList(int nListItem);
	void OnItemChangedPTZPresetList(int nListItem);

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CActivationPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnRadioStateAlarm();
	afx_msg void OnRadioStateOff();
	afx_msg void OnRadioStateOk();
	afx_msg void OnRadioPriority1();
	afx_msg void OnRadioPriority2();
	afx_msg void OnRadioPriority3();
	afx_msg void OnRadioPriority4();
	afx_msg void OnRadioPriority5();
	afx_msg void OnItemchangedListActivation(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnColumnclickListActivation(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkListActivation(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickListActivation(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclickListActivation(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRdblclkListActivation(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	virtual void SaveChanges();
	virtual void CancelChanges();
	virtual void Initialize();

	void    EnableExceptNew(BOOL bAdditional = TRUE);
	void	ShowHideAdditional(BOOL bEnable,CProcess::ProcessType typ, CSecID idArchiv);
	void    SelectActivation(int i, BOOL bSetSel = TRUE);

	void	ClearActivations();
	void	FillActivations();
	int		InsertActivation(const CInput* pInput, CInputToOutput* pIO);
	void	FillTimers();
	void	InsertTimer(const CSecTimer* pTimer);
	void	FillOutputs();
	void	InsertOutput(const COutput* pOutput);
	void	InsertMedia(const CMedia* pMedia);
//	void	FillProcesses();
	void	FillProcesses(const COutput* pOutput,const CMedia* pMedia);
	void	InsertProcess(const CProcess* pProcess);
	void	FillArchivs(BOOL bOnlyAlarmList);
	void	FillNotifications();
	void	FillPTZPresets();
	CString UpdateProcessSettings();

	void SelectedItemToControl();

// Dialog Data
	//{{AFX_DATA(CActivationPage)
	enum { IDD = IDD_ACTIVATION };
	CButton	m_radioPriority1;
	CButton	m_radioPriority2;
	CButton	m_radioPriority3;
	CButton	m_radioPriority4;
	CButton	m_radioPriority5;
	CButton m_radioOK;
	CButton m_radioALARM;
	CButton m_radioOFF;
	CStatic	m_txtNotification;
	CStatic	m_txtArchiv;
	CStatic m_txtPTZPreset;
	CStatusList	m_Notifications;
	CStatusList	m_Archivs;
	CStatusList	m_PTZPresets;
	CStatusList	m_Timers;
	CStatusList	m_Processes;
	CStatusList	m_Outputs;
	CListCtrl	m_InputsToOutputs;
	//}}AFX_DATA

	// implementation
protected:
	static int CALLBACK MyCompareFunction(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

	// data member
protected:
	CInputList*					m_pInputList;
	CSecTimerArray*				m_pTimerList;
	COutputList*				m_pOutputList;
	CMediaList*					m_pMediaList;
	CProcessList*				m_pProcessList;
	CArchivInfoArray*			m_pArchivList;
	CNotificationMessageArray*	m_pNotificationList;

	int				m_iSelectedItem;
	CInputToOutput* m_pSelectedITO; 
	CBitmap			m_headerArrowBitmap;
	int				m_iSelectedColumn;
	
	BOOL			m_bWeAreDeletingDoNotSelect;
	BOOL			m_bEnableAudio;
	CImageList		m_ArchiveImages;
};

#endif // ActivationPage_H

/////////////////////////////////////////////////////////////////////////////
// end of $WORKFILE:$
